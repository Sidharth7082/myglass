#include "GlassPassElement.hpp"
#include "GlassDecoration.hpp"
#include "Globals.hpp"
#include "WindowGeometry.hpp"

CGlassPassElement::CGlassPassElement(const SGlassPassData& data)
    : m_data(data) {}

std::vector<UP<IPassElement>> CGlassPassElement::draw() {
    if (m_data.decoration.valid())
        m_data.decoration->renderPass(g_pHyprRenderer->m_renderData.pMonitor.lock(), m_data.alpha);

    return {};
}

std::optional<CBox> CGlassPassElement::boundingBox() {
    if (!m_data.decoration.valid())
        return std::nullopt;

    auto window = m_data.decoration->getOwner();
    if (!window)
        return std::nullopt;

    const auto monitor = g_pHyprRenderer->m_renderData.pMonitor.lock();
    auto box = WindowGeometry::computeWindowBox(window, monitor);
    if (!box || !monitor)
        return std::nullopt;

    // IPassElement::boundingBox() is defined in monitor-local LOGICAL
    // coordinates; computeWindowBox() returns framebuffer pixels. Convert
    // before expanding with the (logical) sampling padding so Hyprland's
    // pass simplification / live-blur region math scales correctly.
    const float scale = monitor->m_scale > 0.0f ? monitor->m_scale : 1.0f;
    box->scale(1.0 / scale).expand(GlassRenderer::SAMPLE_PADDING_PX / scale).noNegativeSize().round();

    if (!std::isfinite(box->x) || !std::isfinite(box->y) || !std::isfinite(box->w) || !std::isfinite(box->h) || box->w <= 0.0 ||
        box->h <= 0.0)
        return std::nullopt;

    return box;
}

bool CGlassPassElement::needsLiveBlur() {
    // Windows need live blur so the render pass fully re-renders the
    // background behind the glass before we sample it. Without this,
    // partial damage (e.g. typing in a window below) leaves stale pixels
    // in the padded sampling region, causing blinking artifacts.
    // Layers don't need this — they have their own blur cache with
    // scene generation tracking.
    //
    // Hyprland requires every live-blur element to expose a bounding box
    // (RASSERT in CRenderPass::render), so only request live blur while the
    // geometry is actually available.
    return m_data.decoration.valid() && m_data.decoration->getOwner() && boundingBox().has_value();
}

bool CGlassPassElement::needsPrecomputeBlur() {
    return false;
}

bool CGlassPassElement::disableSimplification() {
    return needsLiveBlur();
}
