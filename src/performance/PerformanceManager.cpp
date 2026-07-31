#include "PerformanceManager.hpp"
#include "../Globals.hpp"

#include <cstdlib>
#include <cstdio>
#include <format>
#include <fstream>
#include <unistd.h>
#include <algorithm>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/helpers/Color.hpp>

CPerformanceManager::CPerformanceManager() {
    m_lastLogTime = std::chrono::steady_clock::now();

    const char* telemEnv = std::getenv("MYGLASS_ENABLE_TELEMETRY");
    if (telemEnv && *telemEnv && std::string_view{telemEnv} == "0") {
        m_telemetryEnabled = false;
    }

    const char* logEnv = std::getenv("MYGLASS_ENABLE_TELEMETRY_LOG");
    if (logEnv && *logEnv && std::string_view{logEnv} != "0") {
        m_loggingEnabled = true;
    }
}

void CPerformanceManager::beginFrame() {
    if (!m_telemetryEnabled || m_frameActive)
        return;

    m_metrics.resetFrameCounters();

    m_gpuTimer.initialize();
    m_gpuTimer.updateResults();

    m_cpuTimer.start();
    m_gpuTimer.beginFrame();

    m_frameActive = true;
}

void CPerformanceManager::endFrame() {
    if (!m_telemetryEnabled || !m_frameActive)
        return;

    m_gpuTimer.endFrame();
    m_cpuTimer.stop();

    m_metrics.cpuFrameTimeMs = m_cpuTimer.elapsedMs();
    m_metrics.gpuFrameTimeMs = m_gpuTimer.lastGpuTimeMs();

    updateRamUsage();
    logBenchmarkReport();

    m_frameActive = false;
}

void CPerformanceManager::resetFrameCounters() noexcept {
    m_metrics.resetFrameCounters();
}

void CPerformanceManager::recordDamageAnalysis(const Hyprutils::Math::CRegion& damageRegion, Hyprutils::Math::Vector2D monitorSize) noexcept {
    if (!m_telemetryEnabled)
        return;

    const auto rects = damageRegion.getRects();
    m_metrics.damageRectCount += static_cast<uint32_t>(rects.size());

    size_t totalArea = 0;
    size_t maxArea   = 0;
    int unionX1 = 10000000, unionY1 = 10000000, unionX2 = -10000000, unionY2 = -10000000;

    for (const auto& box : rects) {
        int w = box.x2 - box.x1;
        int h = box.y2 - box.y1;
        if (w > 0 && h > 0) {
            size_t area = static_cast<size_t>(w) * static_cast<size_t>(h);
            totalArea += area;
            if (area > maxArea) maxArea = area;

            if (box.x1 < unionX1) unionX1 = box.x1;
            if (box.y1 < unionY1) unionY1 = box.y1;
            if (box.x2 > unionX2) unionX2 = box.x2;
            if (box.y2 > unionY2) unionY2 = box.y2;
        }
    }

    m_metrics.totalDamagedPixelArea += totalArea;
    if (maxArea > m_metrics.maxDamageRectArea)
        m_metrics.maxDamageRectArea = maxArea;

    if (unionX2 > unionX1 && unionY2 > unionY1) {
        size_t unionArea = static_cast<size_t>(unionX2 - unionX1) * static_cast<size_t>(unionY2 - unionY1);
        m_metrics.boundingUnionArea += unionArea;
    }

    if (m_metrics.boundingUnionArea > 0) {
        m_metrics.unionEfficiency = static_cast<float>(static_cast<double>(m_metrics.totalDamagedPixelArea) / static_cast<double>(m_metrics.boundingUnionArea));
    }

    double monitorArea = monitorSize.x * monitorSize.y;
    if (monitorArea > 0.0) {
        m_metrics.damagedMonitorPct = static_cast<float>((static_cast<double>(totalArea) / monitorArea) * 100.0);
    }
}

void CPerformanceManager::updateRamUsage() noexcept {
    // Read Resident Set Size (RSS estimate in bytes) from /proc/self/statm on Linux
    std::ifstream statm("/proc/self/statm");
    if (statm.is_open()) {
        size_t totalPages = 0, rssPages = 0;
        if (statm >> totalPages >> rssPages) {
            long pageSize = ::sysconf(_SC_PAGESIZE);
            if (pageSize <= 0) pageSize = 4096;
            m_metrics.ramBytes = rssPages * static_cast<size_t>(pageSize);
        }
    }
}

void CPerformanceManager::logBenchmarkReport() {
    if (!m_loggingEnabled)
        return;

    const auto now = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed = now - m_lastLogTime;
    if (elapsed.count() < 1.0)
        return;

    m_lastLogTime = now;

    const double ramMb  = static_cast<double>(m_metrics.ramBytes)  / (1024.0 * 1024.0);
    const double vramMb = static_cast<double>(m_metrics.vramBytes) / (1024.0 * 1024.0);

    const std::string report = std::format(
        "CPU Frame: {:.2f} ms | GPU Frame: {:.2f} ms | Draw Calls: {} | Blur Passes: {}\n"
        "FBO Binds: {} | FBO Allocs: {} | Shader Binds: {} | Texture Uploads: {}\n"
        "Uniform Uploads: {} | Windows: {} | Layers: {} | Damage Regions: {}\n"
        "Damage Rects: {} | Damaged Pixels: {} | Max Rect: {} px | Union Area: {} px | Union Eff: {:.2f} | Damaged Mon: {:.1f}%\n"
        "VRAM (FBO est.): {:.1f} MB | RAM (RSS est.): {:.1f} MB | Heap Allocs: {}",
        m_metrics.cpuFrameTimeMs, m_metrics.gpuFrameTimeMs, m_metrics.drawCalls, m_metrics.blurPasses,
        m_metrics.framebufferBinds, m_metrics.framebufferAllocations, m_metrics.shaderBinds, m_metrics.textureUploads,
        m_metrics.uniformUploads, m_metrics.windowsRendered, m_metrics.layersRendered, m_metrics.damageRegions,
        m_metrics.damageRectCount, m_metrics.totalDamagedPixelArea, m_metrics.maxDamageRectArea,
        m_metrics.boundingUnionArea, m_metrics.unionEfficiency, m_metrics.damagedMonitorPct,
        vramMb, ramMb, m_metrics.heapAllocations
    );

    if (PHANDLE) {
        HyprlandAPI::addNotificationV2(PHANDLE, {
            {"text", report},
            {"time", static_cast<uint64_t>(1000)},
            {"color", CHyprColor{0.2, 0.8, 0.4, 1.0}},
        });
    }
}
