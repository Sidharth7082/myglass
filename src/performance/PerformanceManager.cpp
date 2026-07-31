#include "PerformanceManager.hpp"
#include "../Globals.hpp"

#include <cstdlib>
#include <cstdio>
#include <format>
#include <fstream>
#include <unistd.h>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/helpers/Color.hpp>

CPerformanceManager::CPerformanceManager() {
    m_lastLogTime = std::chrono::steady_clock::now();

    const char* logEnv = std::getenv("MYGLASS_ENABLE_TELEMETRY_LOG");
    if (logEnv && *logEnv && std::string_view{logEnv} != "0") {
        m_loggingEnabled = true;
    }
}

void CPerformanceManager::beginFrame() {
    if (m_frameActive)
        return;

    m_metrics.resetFrameCounters();

    m_gpuTimer.initialize();
    m_gpuTimer.updateResults();

    m_cpuTimer.start();
    m_gpuTimer.beginFrame();

    m_frameActive = true;
}

void CPerformanceManager::endFrame() {
    if (!m_frameActive)
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

void CPerformanceManager::updateRamUsage() noexcept {
    // Read resident set size (RSS) from /proc/self/statm on Linux
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
        "VRAM: {:.1f} MB | RAM: {:.1f} MB | Heap Allocs: {}",
        m_metrics.cpuFrameTimeMs, m_metrics.gpuFrameTimeMs, m_metrics.drawCalls, m_metrics.blurPasses,
        m_metrics.framebufferBinds, m_metrics.framebufferAllocations, m_metrics.shaderBinds, m_metrics.textureUploads,
        m_metrics.uniformUploads, m_metrics.windowsRendered, m_metrics.layersRendered, m_metrics.damageRegions,
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
