#pragma once

#include <chrono>

class CCpuTimer {
public:
    CCpuTimer() = default;

    void start() noexcept {
        m_startTime = std::chrono::steady_clock::now();
        m_running = true;
    }

    void stop() noexcept {
        m_endTime = std::chrono::steady_clock::now();
        m_running = false;
    }

    [[nodiscard]] double elapsedMs() const noexcept {
        const auto end = m_running ? std::chrono::steady_clock::now() : m_endTime;
        const std::chrono::duration<double, std::milli> duration = end - m_startTime;
        return duration.count();
    }

private:
    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::steady_clock::time_point m_endTime{};
    bool m_running = false;
};
