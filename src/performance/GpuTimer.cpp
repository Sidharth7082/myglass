#include "GpuTimer.hpp"

#ifndef GL_TIME_ELAPSED
#define GL_TIME_ELAPSED 0x88BF
#endif

CGpuTimer::CGpuTimer() = default;

CGpuTimer::~CGpuTimer() {
    if (m_initialized && m_supported) {
        glDeleteQueries(static_cast<GLsizei>(m_queries.size()), m_queries.data());
    }
}

void CGpuTimer::initialize() {
    if (m_initialized)
        return;

    glGenQueries(static_cast<GLsizei>(m_queries.size()), m_queries.data());
    if (glGetError() == GL_NO_ERROR) {
        m_supported = true;
    }

    m_initialized = true;
}

void CGpuTimer::beginFrame() {
    if (!m_supported)
        return;

    if (m_activeQuery)
        return;

    GLuint query = m_queries[m_writeIndex];
    glBeginQuery(GL_TIME_ELAPSED, query);
    if (glGetError() == GL_NO_ERROR) {
        m_activeQuery = true;
    }
}

void CGpuTimer::endFrame() {
    if (!m_supported || !m_activeQuery)
        return;

    glEndQuery(GL_TIME_ELAPSED);
    m_activeQuery = false;

    m_writeIndex = (m_writeIndex + 1) % QUERY_BUFFER_SIZE;
}

void CGpuTimer::updateResults() {
    if (!m_supported)
        return;

    // Check if the query at m_readIndex has its result ready
    while (m_readIndex != m_writeIndex) {
        GLuint query = m_queries[m_readIndex];
        GLuint available = GL_FALSE;
        glGetQueryObjectuiv(query, GL_QUERY_RESULT_AVAILABLE, &available);

        if (available == GL_TRUE) {
            GLuint elapsedNs = 0;
            glGetQueryObjectuiv(query, GL_QUERY_RESULT, &elapsedNs);
            m_lastGpuTimeMs = static_cast<double>(elapsedNs) / 1000000.0;
            m_readIndex = (m_readIndex + 1) % QUERY_BUFFER_SIZE;
        } else {
            // Result not ready yet; do not block or stall CPU
            break;
        }
    }
}
