#pragma once

#include <cstddef>
#include <cstdint>

struct PerformanceMetrics {
    // Frame timings (in milliseconds)
    double cpuFrameTimeMs = 0.0;
    double gpuFrameTimeMs = 0.0;

    // Per-frame counters
    uint32_t drawCalls              = 0;
    uint32_t blurPasses             = 0;
    uint32_t framebufferBinds       = 0;
    uint32_t framebufferAllocations = 0;
    uint32_t textureUploads         = 0;
    uint32_t shaderBinds            = 0;
    uint32_t uniformUploads         = 0;
    uint32_t windowsRendered        = 0;
    uint32_t layersRendered         = 0;
    uint32_t damageRegions          = 0;

    // Memory metrics
    size_t ramBytes  = 0;
    size_t vramBytes = 0;

    // Debug metrics
    uint32_t heapAllocations = 0;

    void resetFrameCounters() noexcept {
        cpuFrameTimeMs         = 0.0;
        gpuFrameTimeMs         = 0.0;
        drawCalls              = 0;
        blurPasses             = 0;
        framebufferBinds       = 0;
        framebufferAllocations = 0;
        textureUploads         = 0;
        shaderBinds            = 0;
        uniformUploads         = 0;
        windowsRendered        = 0;
        layersRendered         = 0;
        damageRegions          = 0;
        heapAllocations        = 0;
    }
};
