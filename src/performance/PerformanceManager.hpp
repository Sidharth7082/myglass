#pragma once

#include "CpuTimer.hpp"
#include "GpuTimer.hpp"
#include "PerformanceMetrics.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <hyprutils/math/Region.hpp>
#include <hyprutils/math/Vector2D.hpp>

struct FrameSample {
    double cpuMs = 0.0;
    double gpuMs = 0.0;
    uint32_t damageRects = 0;
    size_t damagedPixels = 0;
    float unionEff = 0.0f;
};

class CPerformanceManager {
public:
    static CPerformanceManager& instance() noexcept {
        static CPerformanceManager mgr;
        return mgr;
    }

    CPerformanceManager(const CPerformanceManager&) = delete;
    CPerformanceManager& operator=(const CPerformanceManager&) = delete;

    void beginFrame();
    void endFrame();
    void resetFrameCounters() noexcept;

    [[nodiscard]] const PerformanceMetrics& getMetrics() const noexcept { return m_metrics; }

    // Telemetry enable/disable control
    void setTelemetryEnabled(bool enabled) noexcept { m_telemetryEnabled = enabled; }
    [[nodiscard]] bool isTelemetryEnabled() const noexcept { return m_telemetryEnabled; }

    void setLoggingEnabled(bool enabled) noexcept { m_loggingEnabled = enabled; }
    [[nodiscard]] bool isLoggingEnabled() const noexcept { return m_loggingEnabled; }

    [[nodiscard]] bool isDebugOverlayEnabled() const noexcept { return m_debugOverlayEnabled; }

    // Instrumentation recording methods with zero-overhead early exit when disabled
    inline void recordDrawCall(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.drawCalls += count;
    }
    inline void recordBlurPass(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.blurPasses += count;
    }
    inline void recordBlitPixelsProcessedEst(size_t count) noexcept {
        if (m_telemetryEnabled) m_metrics.blitPixelsProcessedEst += count;
    }
    inline void recordBlurPixelsProcessedEst(size_t count, size_t fullFramebufferPixels = 0) noexcept {
        if (!m_telemetryEnabled) return;
        m_metrics.blurPixelsProcessedEst += count;
        if (fullFramebufferPixels > 0) {
            m_metrics.scissoredBlurCoveragePct = static_cast<float>((static_cast<double>(count) / static_cast<double>(fullFramebufferPixels)) * 100.0);
            m_metrics.blurReductionPct = std::max(0.0f, 100.0f - m_metrics.scissoredBlurCoveragePct);
        }
    }
    inline void recordFramebufferBind(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.framebufferBinds += count;
    }
    inline void recordShaderBind(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.shaderBinds += count;
    }
    inline void recordUniformUpload(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.uniformUploads += count;
    }
    inline void recordWindowRendered(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.windowsRendered += count;
    }
    inline void recordLayerRendered(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.layersRendered += count;
    }
    inline void recordDamageRegion(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.damageRegions += count;
    }
    inline void recordHeapAllocation(uint32_t count = 1) noexcept {
        if (m_telemetryEnabled) m_metrics.heapAllocations += count;
    }

    // Phase 3.1: Damage analysis telemetry collection & debug overlay
    void recordDamageAnalysis(const Hyprutils::Math::CRegion& damageRegion, Hyprutils::Math::Vector2D monitorSize) noexcept;
    void renderDamageOverlay(const Hyprutils::Math::CRegion& damageRegion) noexcept;

    // Memory metric tracking (VRAM allocation estimates)
    inline void recordFramebufferAllocation(size_t bytes) noexcept {
        if (!m_telemetryEnabled) return;
        m_metrics.framebufferAllocations++;
        m_metrics.vramBytes += bytes;
    }

    inline void recordFramebufferDeallocation(size_t bytes) noexcept {
        if (!m_telemetryEnabled) return;
        m_metrics.vramBytes = (m_metrics.vramBytes >= bytes) ? (m_metrics.vramBytes - bytes) : 0;
    }

    inline void recordTextureUpload(size_t bytes) noexcept {
        if (!m_telemetryEnabled) return;
        m_metrics.textureUploads++;
        m_metrics.vramBytes += bytes;
    }

    inline void recordTextureDestruction(size_t bytes) noexcept {
        if (!m_telemetryEnabled) return;
        m_metrics.vramBytes = (m_metrics.vramBytes >= bytes) ? (m_metrics.vramBytes - bytes) : 0;
    }

private:
    CPerformanceManager();
    ~CPerformanceManager() = default;

    void updateRamUsage() noexcept;
    void logBenchmarkReport();

    PerformanceMetrics m_metrics{};
    CCpuTimer m_cpuTimer{};
    CGpuTimer m_gpuTimer{};

    static constexpr size_t SAMPLE_HISTORY_SIZE = 120;
    std::array<FrameSample, SAMPLE_HISTORY_SIZE> m_sampleHistory{};
    size_t m_sampleIndex = 0;
    size_t m_sampleCount = 0;

    std::chrono::steady_clock::time_point m_lastLogTime{};
    bool m_telemetryEnabled = true;
    bool m_loggingEnabled = false;
    bool m_debugOverlayEnabled = false;
    bool m_frameActive = false;
};
