#pragma once

#include "CpuTimer.hpp"
#include "GpuTimer.hpp"
#include "PerformanceMetrics.hpp"

#include <chrono>

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

    // Instrumentation recording methods
    void recordDrawCall(uint32_t count = 1) noexcept { m_metrics.drawCalls += count; }
    void recordBlurPass(uint32_t count = 1) noexcept { m_metrics.blurPasses += count; }
    void recordFramebufferBind(uint32_t count = 1) noexcept { m_metrics.framebufferBinds += count; }
    void recordShaderBind(uint32_t count = 1) noexcept { m_metrics.shaderBinds += count; }
    void recordUniformUpload(uint32_t count = 1) noexcept { m_metrics.uniformUploads += count; }
    void recordWindowRendered(uint32_t count = 1) noexcept { m_metrics.windowsRendered += count; }
    void recordLayerRendered(uint32_t count = 1) noexcept { m_metrics.layersRendered += count; }
    void recordDamageRegion(uint32_t count = 1) noexcept { m_metrics.damageRegions += count; }
    void recordHeapAllocation(uint32_t count = 1) noexcept { m_metrics.heapAllocations += count; }

    void recordFramebufferAllocation(size_t bytes) noexcept {
        m_metrics.framebufferAllocations++;
        m_metrics.vramBytes += bytes;
    }

    void recordFramebufferDeallocation(size_t bytes) noexcept {
        if (m_metrics.vramBytes >= bytes)
            m_metrics.vramBytes -= bytes;
        else
            m_metrics.vramBytes = 0;
    }

    void recordTextureUpload(size_t bytes) noexcept {
        m_metrics.textureUploads++;
        m_metrics.vramBytes += bytes;
    }

    void recordTextureDestruction(size_t bytes) noexcept {
        if (m_metrics.vramBytes >= bytes)
            m_metrics.vramBytes -= bytes;
        else
            m_metrics.vramBytes = 0;
    }

    void setLoggingEnabled(bool enabled) noexcept { m_loggingEnabled = enabled; }
    [[nodiscard]] bool isLoggingEnabled() const noexcept { return m_loggingEnabled; }

private:
    CPerformanceManager();
    ~CPerformanceManager() = default;

    void updateRamUsage() noexcept;
    void logBenchmarkReport();

    PerformanceMetrics m_metrics{};
    CCpuTimer m_cpuTimer{};
    CGpuTimer m_gpuTimer{};

    std::chrono::steady_clock::time_point m_lastLogTime{};
    bool m_loggingEnabled = false;
    bool m_frameActive = false;
};
