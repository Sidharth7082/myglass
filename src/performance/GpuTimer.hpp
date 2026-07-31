#pragma once

#include <GLES3/gl32.h>
#include <array>
#include <cstddef>
#include <cstdint>

class CGpuTimer {
public:
    CGpuTimer();
    ~CGpuTimer();

    CGpuTimer(const CGpuTimer&) = delete;
    CGpuTimer& operator=(const CGpuTimer&) = delete;

    void initialize();
    void beginFrame();
    void endFrame();

    // Polls asynchronous GPU query results without stalling the pipeline
    void updateResults();

    [[nodiscard]] double lastGpuTimeMs() const noexcept { return m_lastGpuTimeMs; }
    [[nodiscard]] bool isSupported() const noexcept { return m_supported; }

private:
    static constexpr size_t QUERY_BUFFER_SIZE = 3;

    std::array<GLuint, QUERY_BUFFER_SIZE> m_queries{};
    size_t m_writeIndex = 0;
    size_t m_readIndex = 0;

    double m_lastGpuTimeMs = 0.0;
    bool m_initialized = false;
    bool m_supported = false;
    bool m_activeQuery = false;
};
