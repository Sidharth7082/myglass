#include "MyGlassRenderer.hpp"
#include "BuiltInPresets.hpp"
#include "Globals.hpp"
#include "performance/PerformanceManager.hpp"

#include <array>
#include <utility>
#include <GLES3/gl32.h>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>

namespace GlassRenderer {

static GLuint fbId(const SP<Render::IFramebuffer>& framebuffer) {
    return dynamic_cast<Render::GL::CGLFramebuffer*>(framebuffer.get())->getFBID();
}

// ── Perfect-forwarding Uniform wrappers with automatic telemetry recording ──

static inline void setUniform1f(GLint location, float v0) {
    glUniform1f(location, v0);
    CPerformanceManager::instance().recordUniformUpload(1);
}

static inline void setUniform2f(GLint location, float v0, float v1) {
    glUniform2f(location, v0, v1);
    CPerformanceManager::instance().recordUniformUpload(1);
}

static inline void setUniform3f(GLint location, float v0, float v1, float v2) {
    glUniform3f(location, v0, v1, v2);
    CPerformanceManager::instance().recordUniformUpload(1);
}

static inline void setUniform1i(GLint location, int v0) {
    glUniform1i(location, v0);
    CPerformanceManager::instance().recordUniformUpload(1);
}

template <typename ShaderType, typename NameType, typename... Args>
static inline auto setShaderUniformFloat(ShaderType&& shader, NameType&& name, Args&&... args) {
    CPerformanceManager::instance().recordUniformUpload(1);
    return shader->setUniformFloat(std::forward<NameType>(name), std::forward<Args>(args)...);
}

template <typename ShaderType, typename NameType, typename... Args>
static inline auto setShaderUniformFloat2(ShaderType&& shader, NameType&& name, Args&&... args) {
    CPerformanceManager::instance().recordUniformUpload(1);
    return shader->setUniformFloat2(std::forward<NameType>(name), std::forward<Args>(args)...);
}

template <typename ShaderType, typename NameType, typename... Args>
static inline auto setShaderUniformInt(ShaderType&& shader, NameType&& name, Args&&... args) {
    CPerformanceManager::instance().recordUniformUpload(1);
    return shader->setUniformInt(std::forward<NameType>(name), std::forward<Args>(args)...);
}

template <typename ShaderType, typename NameType, typename... Args>
static inline auto setShaderUniformMatrix3fv(ShaderType&& shader, NameType&& name, Args&&... args) {
    CPerformanceManager::instance().recordUniformUpload(1);
    return shader->setUniformMatrix3fv(std::forward<NameType>(name), std::forward<Args>(args)...);
}

static void uploadThemeUniforms(const SResolveContext& ctx) {
    const auto& uniforms = g_pGlobalState->shaderManager.glassUniforms;
    const auto& glassShader = g_pGlobalState->shaderManager.glassShader;
    const auto& defaults = ctx.isDark ? DARK_THEME_DEFAULTS : LIGHT_THEME_DEFAULTS;

    setShaderUniformFloat(glassShader, SHADER_BRIGHTNESS, resolvePresetFloat(ctx, &SPresetValues::brightness, &SOverridableConfig::brightness, defaults.brightness));
    setShaderUniformFloat(glassShader, SHADER_CONTRAST,   resolvePresetFloat(ctx, &SPresetValues::contrast, &SOverridableConfig::contrast, defaults.contrast));
    setUniform1f(uniforms.saturation,                     resolvePresetFloat(ctx, &SPresetValues::saturation, &SOverridableConfig::saturation, defaults.saturation));
    setShaderUniformFloat(glassShader, SHADER_VIBRANCY,   resolvePresetFloat(ctx, &SPresetValues::vibrancy, &SOverridableConfig::vibrancy, defaults.vibrancy));
    setUniform1f(uniforms.vibrancyDarkness,               resolvePresetFloat(ctx, &SPresetValues::vibrancyDarkness, &SOverridableConfig::vibrancyDarkness, defaults.vibrancyDarkness));

    setUniform1f(uniforms.adaptiveDim,   resolvePresetFloat(ctx, &SPresetValues::adaptiveDim, &SOverridableConfig::adaptiveDim, defaults.adaptiveDim));
    setUniform1f(uniforms.adaptiveBoost, resolvePresetFloat(ctx, &SPresetValues::adaptiveBoost, &SOverridableConfig::adaptiveBoost, defaults.adaptiveBoost));
}

void sampleBackground(SP<Render::IFramebuffer>& sampleFramebuffer, SP<Render::IFramebuffer> sourceFramebuffer,
                       CBox box, Vector2D& outPaddingRatio, int downscale,
                       const CBox* pDamageBox) {
    if (!sourceFramebuffer)
        return;
    const int pad = SAMPLE_PADDING_PX;
    int fullWidth  = static_cast<int>(box.width) + 2 * pad;
    int fullHeight = static_cast<int>(box.height) + 2 * pad;

    int sampleWidth  = std::max(1, fullWidth / downscale);
    int sampleHeight = std::max(1, fullHeight / downscale);

    if (!sampleFramebuffer)
        sampleFramebuffer = g_pHyprRenderer->createFB("myglass-sample");

    if (sampleFramebuffer->m_size.x != sampleWidth || sampleFramebuffer->m_size.y != sampleHeight) {
        sampleFramebuffer->alloc(sampleWidth, sampleHeight, sourceFramebuffer->m_drmFormat);
        CPerformanceManager::instance().recordFramebufferAllocation(sampleWidth * sampleHeight * 4);
    }

    int srcX0 = static_cast<int>(box.x) - pad;
    int srcX1 = static_cast<int>(box.x + box.width) + pad;
    int srcY0 = static_cast<int>(box.y) - pad;
    int srcY1 = static_cast<int>(box.y + box.height) + pad;

    int framebufferWidth  = static_cast<int>(sourceFramebuffer->m_size.x);
    int framebufferHeight = static_cast<int>(sourceFramebuffer->m_size.y);

    int dstX0 = 0, dstY0 = 0, dstX1 = sampleWidth, dstY1 = sampleHeight;

    const float xScale = static_cast<float>(sampleWidth) / fullWidth;
    const float yScale = static_cast<float>(sampleHeight) / fullHeight;

    outPaddingRatio = Vector2D(
        static_cast<double>(pad) / fullWidth,
        static_cast<double>(pad) / fullHeight
    );

    // Phase 3.3: Scissored background sampling
    if (pDamageBox && pDamageBox->width > 0 && pDamageBox->height > 0) {
        int cropSrcX0 = std::max(srcX0, static_cast<int>(pDamageBox->x) - pad);
        int cropSrcY0 = std::max(srcY0, static_cast<int>(pDamageBox->y) - pad);
        int cropSrcX1 = std::min(srcX1, static_cast<int>(pDamageBox->x + pDamageBox->width) + pad);
        int cropSrcY1 = std::min(srcY1, static_cast<int>(pDamageBox->y + pDamageBox->height) + pad);

        if (cropSrcX1 > cropSrcX0 && cropSrcY1 > cropSrcY0) {
            int cropDstX0 = static_cast<int>((cropSrcX0 - (box.x - pad)) * xScale);
            int cropDstY0 = static_cast<int>((cropSrcY0 - (box.y - pad)) * yScale);
            int cropDstX1 = static_cast<int>((cropSrcX1 - (box.x - pad)) * xScale);
            int cropDstY1 = static_cast<int>((cropSrcY1 - (box.y - pad)) * yScale);

            cropSrcX0 = std::clamp(cropSrcX0, 0, framebufferWidth);
            cropSrcY0 = std::clamp(cropSrcY0, 0, framebufferHeight);
            cropSrcX1 = std::clamp(cropSrcX1, 0, framebufferWidth);
            cropSrcY1 = std::clamp(cropSrcY1, 0, framebufferHeight);

            cropDstX0 = std::clamp(cropDstX0, 0, sampleWidth);
            cropDstY0 = std::clamp(cropDstY0, 0, sampleHeight);
            cropDstX1 = std::clamp(cropDstX1, 0, sampleWidth);
            cropDstY1 = std::clamp(cropDstY1, 0, sampleHeight);

            if (cropSrcX1 > cropSrcX0 && cropSrcY1 > cropSrcY0 && cropDstX1 > cropDstX0 && cropDstY1 > cropDstY0) {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, fbId(sourceFramebuffer));
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbId(sampleFramebuffer));

                g_pHyprOpenGL->setCapStatus(GL_SCISSOR_TEST, true);
                glScissor(cropDstX0, cropDstY0, cropDstX1 - cropDstX0, cropDstY1 - cropDstY0);

                glBlitFramebuffer(cropSrcX0, cropSrcY0, cropSrcX1, cropSrcY1,
                                  cropDstX0, cropDstY0, cropDstX1, cropDstY1,
                                  GL_COLOR_BUFFER_BIT, GL_LINEAR);

                g_pHyprOpenGL->setCapStatus(GL_SCISSOR_TEST, false);

                CPerformanceManager::instance().recordFramebufferBind(2);
                CPerformanceManager::instance().recordDrawCall(1);
                return;
            }
        }
    }

    if (srcX0 < 0) { dstX0 += static_cast<int>(-srcX0 * xScale); srcX0 = 0; }
    if (srcY0 < 0) { dstY0 += static_cast<int>(-srcY0 * yScale); srcY0 = 0; }
    if (srcX1 > framebufferWidth)  { dstX1 -= static_cast<int>((srcX1 - framebufferWidth) * xScale);  srcX1 = framebufferWidth; }
    if (srcY1 > framebufferHeight) { dstY1 -= static_cast<int>((srcY1 - framebufferHeight) * yScale); srcY1 = framebufferHeight; }

    g_pHyprOpenGL->setCapStatus(GL_SCISSOR_TEST, false);

    glBindFramebuffer(GL_FRAMEBUFFER, fbId(sampleFramebuffer));
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbId(sourceFramebuffer));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbId(sampleFramebuffer));
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                      dstX0, dstY0, dstX1, dstY1,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);

    CPerformanceManager::instance().recordFramebufferBind(3);
    CPerformanceManager::instance().recordDrawCall(1);
}

void blurBackground(SP<Render::IFramebuffer> sampleFramebuffer, float radius, int iterations,
                    SP<Render::IFramebuffer> callerFramebuffer, const CBox* pDamageBox) {
    auto& shaderManager = g_pGlobalState->shaderManager;
    if (!sampleFramebuffer || !callerFramebuffer || radius <= 0.0f || iterations <= 0 || !shaderManager.isInitialized())
        return;

    int width  = static_cast<int>(sampleFramebuffer->m_size.x);
    int height = static_cast<int>(sampleFramebuffer->m_size.y);

    auto& blurTempFramebuffer = g_pGlobalState->blurTempFramebuffer;
    if (!blurTempFramebuffer)
        blurTempFramebuffer = g_pHyprRenderer->createFB("myglass-blur-temp");

    if (blurTempFramebuffer->m_size.x != width || blurTempFramebuffer->m_size.y != height) {
        blurTempFramebuffer->alloc(width, height, sampleFramebuffer->m_drmFormat);
        CPerformanceManager::instance().recordFramebufferAllocation(width * height * 4);
    }

    static constexpr std::array<float, 9> FULLSCREEN_PROJECTION = {
        2.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f,
       -1.0f,-1.0f, 1.0f,
    };

    const auto& blurUniforms = shaderManager.blurUniforms;

    auto shader = g_pHyprOpenGL->useShader(shaderManager.blurShader);
    CPerformanceManager::instance().recordShaderBind(1);

    setShaderUniformMatrix3fv(shader, SHADER_PROJ, 1, GL_FALSE, FULLSCREEN_PROJECTION);
    setShaderUniformInt(shader, SHADER_TEX, 0);
    setUniform1f(blurUniforms.radius, radius);

    glBindVertexArray(shader->getUniformLocation(SHADER_SHADER_VAO));
    g_pHyprOpenGL->setViewport(0, 0, width, height);
    glActiveTexture(GL_TEXTURE0);

    // Phase 3.4: Padded scissored Gaussian blur passes
    bool useScissor = false;
    int sx0 = 0, sy0 = 0, sx1 = 0, sy1 = 0;
    if (pDamageBox && pDamageBox->width > 0 && pDamageBox->height > 0) {
        // Evidence-driven kernel padding formula: ceil(radius * sqrt(iterations)) + 4
        int pad = static_cast<int>(std::ceil(radius * std::sqrt(static_cast<double>(iterations)))) + 4;
        sx0 = std::clamp(static_cast<int>(pDamageBox->x) - pad, 0, width);
        sy0 = std::clamp(static_cast<int>(pDamageBox->y) - pad, 0, height);
        sx1 = std::clamp(static_cast<int>(pDamageBox->x + pDamageBox->width) + pad, 0, width);
        sy1 = std::clamp(static_cast<int>(pDamageBox->y + pDamageBox->height) + pad, 0, height);

        if (sx1 > sx0 && sy1 > sy0) {
            useScissor = true;
            g_pHyprOpenGL->setCapStatus(GL_SCISSOR_TEST, true);
            glScissor(sx0, sy0, sx1 - sx0, sy1 - sy0);
        }
    }

    for (int iteration = 0; iteration < iterations; iteration++) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbId(blurTempFramebuffer));
        sampleFramebuffer->getTexture()->bind();
        setUniform2f(blurUniforms.direction, 1.0f / width, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindFramebuffer(GL_FRAMEBUFFER, fbId(sampleFramebuffer));
        blurTempFramebuffer->getTexture()->bind();
        setUniform2f(blurUniforms.direction, 0.0f, 1.0f / height);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        CPerformanceManager::instance().recordBlurPass(1);
        CPerformanceManager::instance().recordDrawCall(2);
        CPerformanceManager::instance().recordFramebufferBind(2);

        size_t passPixels = useScissor ? (static_cast<size_t>(sx1 - sx0) * static_cast<size_t>(sy1 - sy0)) : (static_cast<size_t>(width) * static_cast<size_t>(height));
        CPerformanceManager::instance().recordBlurPixelsProcessed(passPixels * 2);
    }

    if (useScissor) {
        g_pHyprOpenGL->setCapStatus(GL_SCISSOR_TEST, false);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbId(callerFramebuffer));
    CPerformanceManager::instance().recordFramebufferBind(1);

    glBindVertexArray(0);
    g_pHyprOpenGL->setViewport(0, 0,
        static_cast<int>(callerFramebuffer->m_size.x),
        static_cast<int>(callerFramebuffer->m_size.y));
}

void applyGlassEffect(SP<Render::IFramebuffer> sampleFramebuffer, SP<Render::IFramebuffer> targetFramebuffer,
                       CBox& rawBox, CBox& transformedBox,
                       float alpha, float cornerRadius, float roundingPower,
                       const Vector2D& paddingRatio, const SResolveContext& resolveContext,
                       const SMaskInfo* mask) {
    if (!sampleFramebuffer || !targetFramebuffer)
        return;

    auto& shaderManager  = g_pGlobalState->shaderManager;
    const auto& uniforms = shaderManager.glassUniforms;

    const auto transform = Math::wlTransformToHyprutils(
        Math::invertTransform(g_pHyprRenderer->m_renderData.pMonitor->m_transform));

    Mat3x3 glMatrix = g_pHyprRenderer->projectBoxToTarget(rawBox, transform);
    auto texture    = sampleFramebuffer->getTexture();

    glMatrix.transpose();

    glBindFramebuffer(GL_FRAMEBUFFER, fbId(targetFramebuffer));
    CPerformanceManager::instance().recordFramebufferBind(1);

    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    if (mask && mask->textureId != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(mask->target, mask->textureId);
        glActiveTexture(GL_TEXTURE0);
    }

    auto shader = g_pHyprOpenGL->useShader(shaderManager.glassShader);
    CPerformanceManager::instance().recordShaderBind(1);

    setShaderUniformMatrix3fv(shader, SHADER_PROJ, 1, GL_FALSE, glMatrix.getMatrix());
    setShaderUniformInt(shader, SHADER_TEX, 0);

    const auto fullSize = Vector2D(transformedBox.width, transformedBox.height);
    setShaderUniformFloat2(shader, SHADER_FULL_SIZE,
        static_cast<float>(fullSize.x), static_cast<float>(fullSize.y));

    setUniform1f(uniforms.refractionStrength,  resolvePresetFloat(resolveContext, &SPresetValues::refractionStrength, &SOverridableConfig::refractionStrength));
    setUniform1f(uniforms.chromaticAberration, resolvePresetFloat(resolveContext, &SPresetValues::chromaticAberration, &SOverridableConfig::chromaticAberration));
    setUniform1f(uniforms.fresnelStrength,     resolvePresetFloat(resolveContext, &SPresetValues::fresnelStrength, &SOverridableConfig::fresnelStrength));
    setUniform1f(uniforms.specularStrength,    resolvePresetFloat(resolveContext, &SPresetValues::specularStrength, &SOverridableConfig::specularStrength));
    setUniform1f(uniforms.glassOpacity,        resolvePresetFloat(resolveContext, &SPresetValues::glassOpacity, &SOverridableConfig::glassOpacity) * alpha);
    setUniform1f(uniforms.edgeThickness,       resolvePresetFloat(resolveContext, &SPresetValues::edgeThickness, &SOverridableConfig::edgeThickness));
    setUniform1f(uniforms.lensDistortion,      resolvePresetFloat(resolveContext, &SPresetValues::lensDistortion, &SOverridableConfig::lensDistortion));

    uploadThemeUniforms(resolveContext);

    const int64_t tintColorValue = resolvePresetInt(resolveContext, &SPresetValues::tintColor, &SOverridableConfig::tintColor);
    setUniform3f(uniforms.tintColor,
        static_cast<float>((tintColorValue >> 24) & 0xFF) / 255.0f,
        static_cast<float>((tintColorValue >> 16) & 0xFF) / 255.0f,
        static_cast<float>((tintColorValue >> 8) & 0xFF) / 255.0f);
    setUniform1f(uniforms.tintAlpha,
        static_cast<float>(tintColorValue & 0xFF) / 255.0f);

    setUniform2f(uniforms.uvPadding,
        static_cast<float>(paddingRatio.x),
        static_cast<float>(paddingRatio.y));

    if (mask && mask->textureId != 0) {
        setUniform1i(uniforms.useMask, 1);
        setUniform1i(uniforms.maskTex, 1);
        setUniform2f(uniforms.maskUVOffset,
            static_cast<float>(mask->uvOffset.x),
            static_cast<float>(mask->uvOffset.y));
        setUniform2f(uniforms.maskUVScale,
            static_cast<float>(mask->uvScale.x),
            static_cast<float>(mask->uvScale.y));
        setUniform1f(uniforms.maskAlphaThreshold, mask->alphaThreshold);
    } else {
        setUniform1i(uniforms.useMask, 0);
        setUniform1f(uniforms.maskAlphaThreshold, 0.001f);
    }

    setShaderUniformFloat(shader, SHADER_RADIUS, cornerRadius);
    setShaderUniformFloat(shader, SHADER_ROUNDING_POWER, roundingPower);

    glBindVertexArray(shader->getUniformLocation(SHADER_SHADER_VAO));
    g_pHyprOpenGL->scissor(rawBox);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    g_pHyprOpenGL->scissor(nullptr);

    CPerformanceManager::instance().recordDrawCall(1);
}

} // namespace GlassRenderer
