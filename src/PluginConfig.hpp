#pragma once

#include <hyprland/src/config/shared/Types.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <string>
#include <string_view>
#include <typeinfo>
#include <unordered_map>

inline constexpr std::string_view CONFIG_PREFIX = "plugin:myglass:";

// Window tags for theme and preset selection
inline constexpr std::string_view TAG_THEME_PREFIX  = "myglass_theme_";
inline constexpr std::string_view TAG_PRESET_PREFIX = "myglass_preset_";

// Window tags for per-window enable/disable. Override the global `enabled` setting.
// `myglass_disabled` always wins if both are present.
inline constexpr std::string_view TAG_ENABLED  = "myglass_enabled";
inline constexpr std::string_view TAG_DISABLED = "myglass_disabled";

// Hyprland stores dynamic tags (`tagwindow` dispatcher, dynamic window rules)
// with a trailing '*'. CTagKeeper::isTagged() normalizes this for exact lookups,
// but code iterating getTags() or registering preset names must strip it itself
// so "firefox" and "firefox*" refer to the same preset.
inline std::string_view stripDynamicTagMarker(std::string_view tag) {
    if (tag.ends_with('*'))
        tag.remove_suffix(1);
    return tag;
}

// Sentinel: "not set by user, inherit from parent layer"
inline constexpr Hyprlang::FLOAT SENTINEL_FLOAT = -1.0;
inline constexpr Hyprlang::INT   SENTINEL_INT   = -1;

inline constexpr int MAX_PRESET_INHERITANCE_DEPTH = 8;

namespace ConfigKeys {

// Global-only
inline constexpr auto ENABLED            = "plugin:myglass:enabled";
inline constexpr auto DEFAULT_THEME      = "plugin:myglass:default_theme";
inline constexpr auto DEFAULT_PRESET     = "plugin:myglass:default_preset";
inline constexpr auto MANAGE_WINDOW_BLUR = "plugin:myglass:manage_window_blur";

// Preset keyword, registered as unscoped because Hyprlang does not dispatch
// scoped keyword handlers inside the plugin special category.
inline constexpr auto PRESET_KEYWORD = "preset";

// Overridable — global level
inline constexpr auto BLUR_STRENGTH        = "plugin:myglass:blur_strength";
inline constexpr auto BLUR_ITERATIONS      = "plugin:myglass:blur_iterations";
inline constexpr auto REFRACTION_STRENGTH  = "plugin:myglass:refraction_strength";
inline constexpr auto CHROMATIC_ABERRATION = "plugin:myglass:chromatic_aberration";
inline constexpr auto FRESNEL_STRENGTH     = "plugin:myglass:fresnel_strength";
inline constexpr auto SPECULAR_STRENGTH    = "plugin:myglass:specular_strength";
inline constexpr auto GLASS_OPACITY        = "plugin:myglass:glass_opacity";
inline constexpr auto EDGE_THICKNESS       = "plugin:myglass:edge_thickness";
inline constexpr auto TINT_COLOR           = "plugin:myglass:tint_color";
inline constexpr auto LENS_DISTORTION      = "plugin:myglass:lens_distortion";
inline constexpr auto BRIGHTNESS           = "plugin:myglass:brightness";
inline constexpr auto CONTRAST             = "plugin:myglass:contrast";
inline constexpr auto SATURATION           = "plugin:myglass:saturation";
inline constexpr auto VIBRANCY             = "plugin:myglass:vibrancy";
inline constexpr auto VIBRANCY_DARKNESS    = "plugin:myglass:vibrancy_darkness";
inline constexpr auto ADAPTIVE_DIM          = "plugin:myglass:adaptive_dim";
inline constexpr auto ADAPTIVE_BOOST        = "plugin:myglass:adaptive_boost";

// Layer surface support
inline constexpr auto LAYERS_ENABLED            = "plugin:myglass:layers:enabled";
inline constexpr auto LAYERS_NAMESPACES         = "plugin:myglass:layers:namespaces";
inline constexpr auto LAYERS_EXCLUDE_NAMESPACES = "plugin:myglass:layers:exclude_namespaces";
inline constexpr auto LAYERS_PRESET             = "plugin:myglass:layers:preset";
inline constexpr auto LAYERS_NAMESPACE_PRESETS          = "plugin:myglass:layers:namespace_presets";
inline constexpr auto LAYERS_NAMESPACE_MASK_THRESHOLDS  = "plugin:myglass:layers:namespace_mask_thresholds";

// Overridable — dark theme overrides
inline constexpr auto DARK_BLUR_STRENGTH        = "plugin:myglass:dark:blur_strength";
inline constexpr auto DARK_BLUR_ITERATIONS      = "plugin:myglass:dark:blur_iterations";
inline constexpr auto DARK_REFRACTION_STRENGTH  = "plugin:myglass:dark:refraction_strength";
inline constexpr auto DARK_CHROMATIC_ABERRATION = "plugin:myglass:dark:chromatic_aberration";
inline constexpr auto DARK_FRESNEL_STRENGTH     = "plugin:myglass:dark:fresnel_strength";
inline constexpr auto DARK_SPECULAR_STRENGTH    = "plugin:myglass:dark:specular_strength";
inline constexpr auto DARK_GLASS_OPACITY        = "plugin:myglass:dark:glass_opacity";
inline constexpr auto DARK_EDGE_THICKNESS       = "plugin:myglass:dark:edge_thickness";
inline constexpr auto DARK_TINT_COLOR           = "plugin:myglass:dark:tint_color";
inline constexpr auto DARK_LENS_DISTORTION      = "plugin:myglass:dark:lens_distortion";
inline constexpr auto DARK_BRIGHTNESS           = "plugin:myglass:dark:brightness";
inline constexpr auto DARK_CONTRAST             = "plugin:myglass:dark:contrast";
inline constexpr auto DARK_SATURATION           = "plugin:myglass:dark:saturation";
inline constexpr auto DARK_VIBRANCY             = "plugin:myglass:dark:vibrancy";
inline constexpr auto DARK_VIBRANCY_DARKNESS    = "plugin:myglass:dark:vibrancy_darkness";
inline constexpr auto DARK_ADAPTIVE_DIM          = "plugin:myglass:dark:adaptive_dim";
inline constexpr auto DARK_ADAPTIVE_BOOST        = "plugin:myglass:dark:adaptive_boost";

// Overridable — light theme overrides
inline constexpr auto LIGHT_BLUR_STRENGTH        = "plugin:myglass:light:blur_strength";
inline constexpr auto LIGHT_BLUR_ITERATIONS      = "plugin:myglass:light:blur_iterations";
inline constexpr auto LIGHT_REFRACTION_STRENGTH  = "plugin:myglass:light:refraction_strength";
inline constexpr auto LIGHT_CHROMATIC_ABERRATION = "plugin:myglass:light:chromatic_aberration";
inline constexpr auto LIGHT_FRESNEL_STRENGTH     = "plugin:myglass:light:fresnel_strength";
inline constexpr auto LIGHT_SPECULAR_STRENGTH    = "plugin:myglass:light:specular_strength";
inline constexpr auto LIGHT_GLASS_OPACITY        = "plugin:myglass:light:glass_opacity";
inline constexpr auto LIGHT_EDGE_THICKNESS       = "plugin:myglass:light:edge_thickness";
inline constexpr auto LIGHT_TINT_COLOR           = "plugin:myglass:light:tint_color";
inline constexpr auto LIGHT_LENS_DISTORTION      = "plugin:myglass:light:lens_distortion";
inline constexpr auto LIGHT_BRIGHTNESS           = "plugin:myglass:light:brightness";
inline constexpr auto LIGHT_CONTRAST             = "plugin:myglass:light:contrast";
inline constexpr auto LIGHT_SATURATION           = "plugin:myglass:light:saturation";
inline constexpr auto LIGHT_VIBRANCY             = "plugin:myglass:light:vibrancy";
inline constexpr auto LIGHT_VIBRANCY_DARKNESS    = "plugin:myglass:light:vibrancy_darkness";
inline constexpr auto LIGHT_ADAPTIVE_DIM          = "plugin:myglass:light:adaptive_dim";
inline constexpr auto LIGHT_ADAPTIVE_BOOST        = "plugin:myglass:light:adaptive_boost";

} // namespace ConfigKeys

// Cached pointers for a single config layer (built-in dark/light/global)
struct SOverridableConfig {
    Hyprlang::FLOAT* const* blurStrength        = nullptr;
    Hyprlang::INT* const*   blurIterations      = nullptr;
    Hyprlang::FLOAT* const* refractionStrength  = nullptr;
    Hyprlang::FLOAT* const* chromaticAberration = nullptr;
    Hyprlang::FLOAT* const* fresnelStrength     = nullptr;
    Hyprlang::FLOAT* const* specularStrength    = nullptr;
    Hyprlang::FLOAT* const* glassOpacity        = nullptr;
    Hyprlang::FLOAT* const* edgeThickness       = nullptr;
    Hyprlang::INT* const*   tintColor           = nullptr;
    Hyprlang::FLOAT* const* lensDistortion      = nullptr;
    Hyprlang::FLOAT* const* brightness          = nullptr;
    Hyprlang::FLOAT* const* contrast            = nullptr;
    Hyprlang::FLOAT* const* saturation          = nullptr;
    Hyprlang::FLOAT* const* vibrancy            = nullptr;
    Hyprlang::FLOAT* const* vibrancyDarkness    = nullptr;
    Hyprlang::FLOAT* const* adaptiveDim         = nullptr;
    Hyprlang::FLOAT* const* adaptiveBoost       = nullptr;
};

// Plain values for a user-defined preset layer (all sentinel = not set → inherit)
struct SPresetValues {
    float   blurStrength       = static_cast<float>(SENTINEL_FLOAT);
    int64_t blurIterations     = SENTINEL_INT;
    float   refractionStrength = static_cast<float>(SENTINEL_FLOAT);
    float   chromaticAberration = static_cast<float>(SENTINEL_FLOAT);
    float   fresnelStrength    = static_cast<float>(SENTINEL_FLOAT);
    float   specularStrength   = static_cast<float>(SENTINEL_FLOAT);
    float   glassOpacity       = static_cast<float>(SENTINEL_FLOAT);
    float   edgeThickness      = static_cast<float>(SENTINEL_FLOAT);
    int64_t tintColor          = SENTINEL_INT;
    float   lensDistortion     = static_cast<float>(SENTINEL_FLOAT);
    float   brightness         = static_cast<float>(SENTINEL_FLOAT);
    float   contrast           = static_cast<float>(SENTINEL_FLOAT);
    float   saturation         = static_cast<float>(SENTINEL_FLOAT);
    float   vibrancy           = static_cast<float>(SENTINEL_FLOAT);
    float   vibrancyDarkness   = static_cast<float>(SENTINEL_FLOAT);
    float   adaptiveDim        = static_cast<float>(SENTINEL_FLOAT);
    float   adaptiveBoost      = static_cast<float>(SENTINEL_FLOAT);
};

struct SCustomPreset {
    std::string   name;
    std::string   inherits;
    SPresetValues shared;
    SPresetValues dark;
    SPresetValues light;
};

struct StringConfigPtr {
    void* const*          dataptr = nullptr;
    const std::type_info* type    = nullptr;
};

inline std::string_view readStringConfig(const StringConfigPtr& ptr) {
    if (!ptr.dataptr || !ptr.type)
        return {};

    if (*ptr.type == typeid(Config::STRING)) {
        const auto* value = *reinterpret_cast<Config::STRING* const*>(ptr.dataptr);
        return value ? std::string_view(*value) : std::string_view{};
    }

    if (*ptr.type == typeid(Hyprlang::STRING)) {
        const auto value = *reinterpret_cast<Hyprlang::STRING const*>(ptr.dataptr);
        return value ? std::string_view(value) : std::string_view{};
    }

    return {};
}

struct SPluginConfig {
    Hyprlang::INT* const* enabled          = nullptr;
    // Glass replaces Hyprland's blur for glassed windows: when set, the plugin
    // marks them with the noblur window property so Hyprland composites them
    // against the live framebuffer (which contains the glass) instead of its
    // pre-frame cached blur.
    Hyprlang::INT* const* manageWindowBlur = nullptr;
    StringConfigPtr      defaultTheme;
    StringConfigPtr      defaultPreset;

    Hyprlang::INT* const* layersEnabled                  = nullptr;
    StringConfigPtr       layersNamespaces;
    StringConfigPtr       layersExcludeNamespaces;
    StringConfigPtr       layersPreset;
    StringConfigPtr       layersNamespacePresets;
    StringConfigPtr       layersNamespaceMaskThresholds;

    SOverridableConfig global;
    SOverridableConfig dark;
    SOverridableConfig light;
};

// Context for preset-aware value resolution
struct SResolveContext {
    std::string_view                                      presetName;
    bool                                                  isDark;
    const SPluginConfig&                                  config;
    const std::unordered_map<std::string, SCustomPreset>& customPresets;
};

// Preset-aware resolution: preset chain → built-in theme → global → hardcoded
[[nodiscard]] float resolvePresetFloat(
    const SResolveContext& context,
    float SPresetValues::* presetField,
    Hyprlang::FLOAT* const* SOverridableConfig::* configField,
    float hardcodedDefault = static_cast<float>(SENTINEL_FLOAT));

[[nodiscard]] int64_t resolvePresetInt(
    const SResolveContext& context,
    int64_t SPresetValues::* presetField,
    Hyprlang::INT* const* SOverridableConfig::* configField,
    int64_t hardcodedDefault = SENTINEL_INT);

void registerConfig(HANDLE handle);
void initConfigPointers(HANDLE handle, SPluginConfig& config);

// Preset keyword handler (registered via addConfigKeyword)
Hyprlang::CParseResult handlePresetKeyword(const char* command, const char* value);

// Clear pending presets/layers before config re-parse (called from preConfigReload callback)
void clearPendingPresets();
void clearPendingLayers();

// Swap pending data into active maps (called from configReloaded callback)
void commitPendingPresets();
void commitPendingLayers();

// Validate config values and notify user of misconfigurations
void validateConfig();
