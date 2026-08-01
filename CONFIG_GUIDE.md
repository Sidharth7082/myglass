# 📖 MyGlass Configuration Guide

Comprehensive guide to customizing **MyGlass**, an Apple-inspired Liquid Glass plugin for Hyprland.

---

## 🚀 Basic Setup in Hyprland (Lua)

To configure MyGlass in Hyprland using Lua, add your settings to `~/.config/hypr/module/myglass.lua` (or require it inside `hyprland.lua`).

Always wrap your configuration in a safety check to ensure the plugin is loaded:

```lua
if hl.plugin and hl.plugin.myglass then
    local hg = hl.plugin.myglass

    hg.config({
        enabled = true,
        default_theme = "dark",
        default_preset = "clear",
        glass_opacity = 0.09,
        blur_strength = 0.04,
        tint_color = 0x00000000,
    })
end
```

---

## ⚙️ Core Plugin Settings (`hg.config`)

The `hg.config({...})` table controls global behavior and default glass parameters.

### 1. General Control

| Property | Type | Default | Description |
|---|---|---|---|
| `enabled` | `boolean` | `true` | Globally enables (`true`) or disables (`false`) the MyGlass effect. |
| `default_theme` | `string` | `"dark"` | Active theme mode: `"dark"` or `"light"`. Controls theme-specific overrides. |
| `default_preset` | `string` | `"default"` | Base preset assigned to windows if no per-window preset rule is set. |
| `manage_window_blur` | `boolean` | `true` | Automatically disables Hyprland's native blur on glassed windows to prevent double-blur artifacts. |

---

### 2. Glass Shader Parameters

These properties customize the physical glass appearance, refraction, and optical characteristics:

| Parameter | Type | Default | Range / Example | Description |
|---|---|---|---|---|
| `blur_strength` | `number` | `2.0` | `0.0` - `5.0` | Radius of background blur behind the glass. Set to `0.0` for crystal clear glass. |
| `blur_iterations` | `number` | `3` | `1` - `5` | Quality/pass count of the Gaussian blur algorithm. Higher = smoother blur, more GPU work. |
| `refraction_strength` | `number` | `0.6` | `0.0` - `10.0` | Optical bending / refraction strength of background elements behind the glass. |
| `chromatic_aberration` | `number` | `0.5` | `0.0` - `2.0` | Color dispersion / RGB fringe split along glass curves. |
| `fresnel_strength` | `number` | `0.6` | `0.0` - `1.0` | Edge reflection glint intensity when looking at curved glass edges. |
| `specular_strength` | `number` | `0.8` | `0.0` - `1.0` | Brightness of light highlights on the glass surface. |
| `glass_opacity` | `number` | `1.0` | `0.0` - `1.0` | Overall alpha transparency factor (`0.0` = invisible glass, `1.0` = full effect). |
| `edge_thickness` | `number` | `0.06` | `0.01` - `0.2` | Bevel size / curved border thickness ratio of the glass panel. |
| `tint_color` | `hex int` | `0x8899aa22` | ARGB / RGBA hex | Color tint added to the glass surface (e.g. `0x8899aa22` or `0x00000000` for clear). |
| `lens_distortion` | `number` | `0.5` | `0.0` - `1.0` | Curvature distortion intensity across the window body. |

---

### 3. Color & Tone Adjustments

| Parameter | Type | Description |
|---|---|---|
| `brightness` | `number` | Modifies background image brightness under the glass (e.g. `0.85` for dark glass, `1.1` for bright). |
| `contrast` | `number` | Adjusts image contrast ratio behind the glass panel. |
| `saturation` | `number` | Color saturation multiplier for background pixels. |
| `vibrancy` | `number` | Enhances backdrop color vibrancy. |
| `vibrancy_darkness` | `number` | Controls vibrancy dampening in dark shadow areas. |
| `adaptive_dim` | `number` | Automatically dims bright wallpapers under dark theme glass. |
| `adaptive_boost` | `number` | Automatically boosts contrast on light theme glass. |

---

## 🌓 Dark & Light Theme Overrides

You can specify distinct parameters for **dark** and **light** modes inside `hg.config({...})`:

```lua
hg.config({
    default_theme = "dark",
    glass_opacity = 0.8,

    -- Dark theme overrides
    dark = {
        brightness = 0.82,
        adaptive_dim = 0.4,
        tint_color = 0x02142aa9,
    },

    -- Light theme overrides
    light = {
        brightness = 1.12,
        adaptive_boost = 0.4,
        tint_color = 0xc2cddb33,
    },
})
```

---

## 🎨 Presets System (`hg.preset`)

MyGlass includes built-in presets and allows you to create custom, reusable presets with inheritance.

### Built-in Presets

| Preset Name | Characteristics |
|---|---|
| `"clear"` | `blur_strength = 0.0`, minimal refraction (`0.3`), crystal clear see-through glass. |
| `"subtle"` | Gentle blur (`1.0`), low refraction (`0.3`), soft edge specular highlights (`0.4`). |
| `"glass"` | High refraction (`8.0`), realistic lens distortion (`0.3`), vibrant glass glint. |
| `"high_contrast"` | Medium blur (`1.2`), high contrast (`1.14`), enhanced adaptive dimming (`0.25`). |

### Creating Custom Presets

Use `hg.preset("preset_name", {...})` to define custom presets:

```lua
-- Custom clear glass preset for terminal windows
hg.preset("clear_terminal", {
    glass_opacity = 0.08,
    blur_strength = 1.5,
    refraction_strength = 0.4,
    dark = {
        brightness = 0.75,
    },
    light = {
        brightness = 1.2,
    },
})

-- Preset inheriting from built-in 'high_contrast'
hg.preset("my_thick_glass", {
    inherits = "high_contrast",
    edge_thickness = 0.12,
    refraction_strength = 2.0,
})
```

---

## 🪟 Layer Surface Glass (`hg.layer`)

MyGlass supports glass effects on Wayland layer surfaces like **Waybar**, **SwayNC**, **Quickshell**, or **Rofi**.

### Enable Layer Glass Globally

```lua
hg.config({
    layers = {
        enabled = true,
    },
})
```

### Apply Presets to Specific Layers

Use `hg.layer("namespace", {...})` to configure layer surfaces:

```lua
-- Apply subtle preset to Waybar
hg.layer("waybar", {
    preset = "subtle",
    mask_threshold = 0.05,
})

-- Enable glass for SwayNC notifications
hg.layer("swaync")

-- Custom glass for Dynamic Island
hg.layer("nowoward-capdynamic", {
    preset = "clear",
})

-- Custom glass for Quickshell panels
hg.layer("quickshell:bezel", {
    preset = "clear",
    mask_threshold = 0.3,
})

-- Exclude debug overlay layers from glassing
hg.layer("debug-panel", {
    exclude = true,
})
```

---

## 🏷️ Window Tags & Rules Integration

You can assign MyGlass presets or toggle glass on specific windows using Hyprland window tags:

```lua
-- Force MyGlass ON for Kitty terminal
hl.window_rule({
    match = { class = "^(kitty)$" },
    tag   = "myglass_enabled",
})

-- Assign the 'clear_terminal' preset to Kitty
hl.window_rule({
    match = { class = "^(kitty)$" },
    tag   = "myglass_preset_clear_terminal",
})

-- Disable MyGlass for specific applications
hl.window_rule({
    match = { class = "^(firefox)$" },
    tag   = "myglass_disabled",
})
```

---

## 💡 Complete Example Configuration

Here is a full example configuration for your `~/.config/hypr/module/myglass.lua`:

```lua
if hl.plugin and hl.plugin.myglass then
    local hg = hl.plugin.myglass

    -- 1. Global Plugin Configuration
    hg.config({
        enabled = true,
        default_theme = "dark",
        default_preset = "clear",

        -- Base Glass Parameters
        glass_opacity = 0.09,
        blur_strength = 0.04,
        refraction_strength = 0.6,
        chromatic_aberration = 0.3,
        tint_color = 0x00000000,

        -- Theme Overrides
        dark = {
            brightness = 0.82,
            adaptive_dim = 0.25,
        },
        light = {
            brightness = 1.10,
            adaptive_boost = 0.3,
        },

        -- Enable Layer Surfaces (Waybar, SwayNC)
        layers = {
            enabled = true,
        },
    })

    -- 2. Custom Presets
    hg.preset("frosted_ui", {
        blur_strength = 2.5,
        blur_iterations = 3,
        glass_opacity = 0.85,
        refraction_strength = 1.2,
    })

    -- 3. Layer Rules
    hg.layer("waybar", { preset = "frosted_ui", mask_threshold = 0.05 })
    hg.layer("swaync", { preset = "subtle" })
end
```

---

*For issues or questions, visit the GitHub repository: [Sidharth7082/myglass](https://github.com/Sidharth7082/myglass).*
