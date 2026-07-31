<div align="center">

# 🧊 MyGlass

**A modern, high-performance Apple-style Liquid Glass plugin for [Hyprland](https://hyprland.org).**

[![Build](https://img.shields.io/github/actions/workflow/status/Sidharth7082/myglass/build.yml?branch=main&style=for-the-badge&logo=github)](https://github.com/Sidharth7082/myglass/actions/workflows/build.yml)
[![Release](https://img.shields.io/github/v/release/Sidharth7082/myglass?style=for-the-badge&color=8A2BE2)](https://github.com/Sidharth7082/myglass/releases/latest)
[![Hyprland](https://img.shields.io/badge/Hyprland-v0.56+-00b4d8?style=for-the-badge&logo=archlinux)](https://hyprland.org)
[![Lua API](https://img.shields.io/badge/Lua-First--Class-000080?style=for-the-badge&logo=lua)](https://lua.org)
[![License](https://img.shields.io/github/license/Sidharth7082/myglass?style=for-the-badge&color=brightgreen)](LICENSE)

<br />

[✨ Features](#-features) • [🖼️ Screenshots](#%EF%B8%8F-screenshots) • [⚡ Quick Start](#-quick-start) • [⚙️ Configuration](#%EF%B8%8F-configuration) • [🎛️ Presets](#%EF%B8%8F-presets) • [📜 License](#-license)

---

</div>

## 🌟 Overview

**MyGlass** transforms your Hyprland desktop into a luxury macOS-inspired liquid glass aesthetic. Powered by custom GLSL shaders and signed distance fields (SDF), it computes real-time edge refraction, chromatic aberration, specular highlights, and adaptive luminance adjustments across windows and Wayland layer surfaces.

---

## ✨ Features

- 💧 **Apple-Inspired Liquid Glass**: SDF-driven height field rendering with dynamic edge refraction, chromatic dispersion, and lens curvature.
- ⚡ **Real-Time Blur Pipeline**: Ultra-fast multi-pass Gaussian blur with damage-driven rendering and zero idle GPU overhead.
- 📜 **First-Class Lua API**: Configure effortlessly using Hyprland's native Lua interface via `hl.plugin.myglass`.
- 🎨 **Custom Glass Presets**: Create reusable glass profiles with parameter inheritance.
- 🪟 **Per-Layer Surface Support**: Seamlessly glassify Waybar, SwayNC, Quickshell, Rofi, and desktop widgets.
- 🔄 **Reboot & Session Persistence**: Managed through `hyprpm` for automatic rebuilding and persistence across restarts.
- 🎛️ **Granular Tuning**: Fine-tune tint color, opacity, desaturation, vibrancy, brightness, dark/light themes, and edge thickness.
- 🛡️ **Open Source**: 100% free and open-source under the BSD 3-Clause License.

---

## 🖼️ Screenshots

<div align="center">

| 🖥️ Desktop | 📊 Waybar Surface | 🚀 Launcher / Menu |
| :---: | :---: | :---: |
| ![Desktop](assets/desktop.png) | ![Waybar](assets/waybar.png) | ![Launcher](assets/launcher.png) |

</div>

---

## 📋 Requirements

Before installing, ensure your environment meets the following requirements:

| Component | Requirement |
|---|---|
| **Compositor** | [Hyprland](https://hyprland.org) (v0.55+ / latest) |
| **Plugin Manager** | `hyprpm` (comes bundled with Hyprland) |
| **Build System** | `CMake`, `make`, `pkg-config` |
| **Compiler** | `GCC` or `Clang` with C++23 support |
| **Libraries** | `pixman-1`, `libdrm`, OpenGL / GLES headers |

---

## ⚡ Quick Start

### 1. Install via `hyprpm` (Recommended)

`hyprpm` compiles MyGlass directly against your running Hyprland headers to guarantee ABI binary compatibility:

```bash
# Add repository
hyprpm add https://github.com/Sidharth7082/myglass

# Update & compile against running Hyprland version
hyprpm update

# Enable plugin
hyprpm enable myglass

# Reload Hyprland compositor
hyprctl reload
```

---

### 2. Manual Build from Source

```bash
git clone https://github.com/Sidharth7082/myglass.git
cd myglass

# Compile shared library
make -j$(nproc)

# Load plugin dynamically
hyprctl plugin load $(pwd)/myglass.so
```

To load automatically in your `hyprland.conf`:
```ini
plugin = /path/to/myglass.so
```

---

## ⚙️ Configuration

### Native Lua Configuration (Hyprland 0.55+)

Add the following to your Hyprland Lua config:

```lua
if hl.plugin.myglass then
    local hg = hl.plugin.myglass

    -- Global plugin options
    hg.config({
        default_theme = "dark",
        default_preset = "clear",
        tint_color = 0x8899aa22,

        brightness = 0.9,
        dark = { brightness = 0.82 },
        light = { adaptive_boost = 0.5 },

        layers = { enabled = true },
    })

    -- Layer surface rules (Waybar, SwayNC, Quickshell)
    hg.layer("waybar", { preset = "subtle", mask_threshold = 0.05 })
    hg.layer("swaync")
    hg.layer("quickshell:bezel", { preset = "ui", mask_threshold = 0.3 })
    hg.layer("debug-panel", { exclude = true })

    -- Custom presets
    hg.preset("clear", {
        glass_opacity = 0.8,
        blur_strength = 1.5,
        dark = { brightness = 0.7 },
        light = { brightness = 1.2 },
    })

    hg.preset("contrasted", {
        inherits = "high_contrast",
        contrast = 1.2,
        adaptive_dim = 1.5,
        dark = { tint_color = 0x02142aa9 },
    })
end
```

<details>
<summary><b>Click to expand Legacy .conf Configuration</b></summary>

```ini
plugin:myglass {
    default_theme = dark
    default_preset = clear
    tint_color = 0x8899aa22
    brightness = 0.9

    preset = name:clear, glass_opacity:0.8, blur_strength:1.5
    preset = name:contrasted, inherits:high_contrast, contrast:1.2

    layers {
        enabled = 1
        namespaces = waybar, swaync, quickshell:bezel
        preset = subtle
    }
}
```
</details>

---

## 🎛️ Presets & Window Rules

### Built-in Presets

| Preset | Description | Visual Characteristics |
|---|---|---|
| `clear` | Rounded translucent glass plate | Subtle bezel refraction, crisp background |
| `subtle` | Soft frosted glass | Reduced blur radius, minimal specular highlights |
| `high_contrast` | Vivid punchy glass | High contrast, strong tinting, sharp edge refraction |
| `glass` | Thick glass block | High chromatic aberration, deep refraction lens |

### Per-Window Tag Overrides

You can control glass decoration dynamically per window using Hyprland window tags:

| Tag | Function | Example Usage |
|---|---|---|
| `+myglass_enabled` | Force enable glass on window | `hyprctl dispatch tagwindow +myglass_enabled` |
| `+myglass_disabled` | Disable glass on window | `hyprctl dispatch tagwindow +myglass_disabled` |
| `+myglass_theme_dark` | Set dark theme override | `hyprctl dispatch tagwindow +myglass_theme_dark` |
| `+myglass_theme_light` | Set light theme override | `hyprctl dispatch tagwindow +myglass_theme_light` |
| `+myglass_preset_<name>` | Apply preset to window | `hyprctl dispatch tagwindow +myglass_preset_subtle` |

---

## 🚀 Performance & Optimization

- **Damage-Driven Rendering**: Background blurs are only recalculated when window positions or underlying content change.
- **Cached Glass Textures**: Idle windows render with zero additional GPU draw calls.
- **Tuning for Low-End GPUs**: Reduce `blur_iterations` from `3` to `1` or `2` for a significant boost on integrated graphics.

---

## 🔧 Troubleshooting

<details>
<summary><b>ABI Version Mismatch Warning</b></summary>

If Hyprland displays an ABI version mismatch after updating the compositor, rebuild MyGlass against the running version:
```bash
hyprpm update
```
</details>

<details>
<summary><b>Glass Effect Not Displaying</b></summary>

1. Ensure window transparency/opacity is enabled in your window manager rules.
2. If `manage_window_blur = false`, verify compositor blur caching doesn't block plugin decorations (`windowrule = noblur, <window>`).
</details>

---

## 👤 Author & Credits

Created and maintained by **[Sidharth7082](https://github.com/Sidharth7082)** (Capture).

---

## 📄 License

This project is licensed under the [BSD 3-Clause License](LICENSE) - see the LICENSE file for details.
