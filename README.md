<div align="center">

# 🧊 MyGlass

### **Apple-Style Liquid Glass Effects for Hyprland**

*Make your windows look like shiny frosted liquid glass in seconds!*

[![Build Status](https://img.shields.io/github/actions/workflow/status/Sidharth7082/myglass/build.yml?branch=main&style=for-the-badge&logo=github)](https://github.com/Sidharth7082/myglass/actions/workflows/build.yml)
[![Configuration Guide](https://img.shields.io/badge/Documentation-Config%20Guide-FF4500?style=for-the-badge&logo=bookstack&logoColor=white)](CONFIG_GUIDE.md)

---

</div>

## 🚀 1-Minute Easy Installation (Copy & Paste)

Just open your **Terminal** and copy-paste these commands!

### ⚡ Option A: The All-In-One Single Command (Fastest)

Copy and paste this **single line** into your terminal and press `Enter`:

```bash
hyprpm add https://github.com/Sidharth7082/myglass && hyprpm update && hyprpm enable myglass && hyprctl reload
```

🎉 **That's it! MyGlass is installed and working!**

---

### 📋 Option B: Step-by-Step Installation

If you prefer doing it step-by-step:

#### **Step 1:** Add MyGlass to Hyprland
```bash
hyprpm add https://github.com/Sidharth7082/myglass
```

#### **Step 2:** Download & Build
```bash
hyprpm update
```

#### **Step 3:** Enable MyGlass
```bash
hyprpm enable myglass
```

#### **Step 4:** Reload Hyprland
```bash
hyprctl reload
```

---

## 🖼️ Screenshots

<div align="center">

| 🖥️ Desktop | 📊 Waybar Surface |
| :---: | :---: |
| ![Desktop](assets/desktop.png) | ![Waybar](assets/waybarimg.png) |

| 📈 btop System Monitor (Liquid Glass) | 🎛️ Pavucontrol (Smoked Glass HUD) |
| :---: | :---: |
| ![btop Glass](assets/btop.png) | ![Pavucontrol HUD](assets/pavucontrol_hud.png) |

</div>

---

## 🎛️ Pavucontrol (Smoked Glass HUD Theme)

Give your **Pavucontrol** volume manager a futuristic **Smoked Glass HUD** interface that matches your terminal and desktop wallpaper!

### ⚡ 1-Click Installation Command
Run this single command in your terminal to automatically install the GTK theme and Hyprland glass transparency rules:

```bash
curl -fsSL https://raw.githubusercontent.com/Sidharth7082/myglass/main/themes/pavucontrol/install.sh | bash
```

---

## 🎮 How to Use It (Fun Commands)

Want to turn glass on or off for a specific window or toggle modes live? Copy & paste these into your terminal!

### 🚫 Turn glass OFF on the current window:
```bash
hyprctl dispatch tagwindow +myglass_disabled
```

### ✨ Turn glass ON on the current window:
```bash
hyprctl dispatch tagwindow +myglass_enabled
```

### 💎 Change style to "Clear Glass" (Recommended Default):
```bash
echo "clear" > ~/.config/hypr/myglass_preset.state && hyprctl reload
```

### 💻 Change style to "Terminal Glass" (For btop & Terminals):
```bash
echo "terminal_glass" > ~/.config/hypr/myglass_preset.state && hyprctl reload
```

### 🎨 Change style to "Subtle Glass":
```bash
echo "subtle" > ~/.config/hypr/myglass_preset.state && hyprctl reload
```

### ❄️ Change style to "Acrylic Glass":
```bash
echo "acrylic" > ~/.config/hypr/myglass_preset.state && hyprctl reload
```

### ⚡ Change style to "High Contrast Glass":
```bash
echo "high_contrast" > ~/.config/hypr/myglass_preset.state && hyprctl reload
```

---

## 🎨 Available Presets Overview

| Preset | Character & Style | Best Used For | Command |
|---|---|---|---|
| 💎 **`clear`** *(Recommended)* | Crystal clear see-through glass with zero blur distortion. | Wallpaper transparency, aesthetic desktop setups. | `echo "clear" > ~/.config/hypr/myglass_preset.state && hyprctl reload` |
| 💻 **`terminal_glass`** | High text contrast with low distortion & crisp text. | Terminal windows, `btop`, Neovim, code editors. | `echo "terminal_glass" > ~/.config/hypr/myglass_preset.state && hyprctl reload` |
| 🎨 **`subtle`** | Gentle background blur (`1.0`) with soft edge glint. | Daily app windows, file managers. | `echo "subtle" > ~/.config/hypr/myglass_preset.state && hyprctl reload` |
| ❄️ **`acrylic`** | Heavy Windows-style acrylic frosted diffusion (`3.5` blur). | Floating panels, popups, sidebars. | `echo "acrylic" > ~/.config/hypr/myglass_preset.state && hyprctl reload` |
| ⚡ **`high_contrast`** | Enhanced contrast multiplier & adaptive dimming. | Bright wallpapers & white themes. | `echo "high_contrast" > ~/.config/hypr/myglass_preset.state && hyprctl reload` |

## 🔄 Auto-Load On Every Startup

To keep MyGlass automatically active every time Hyprland starts:

### 📜 Lua Configuration (`autostart.lua`)
Add `hl.exec_cmd("hyprpm reload -n")` inside your autostart handler:
```lua
hl.on("hyprland.start", function ()
    hl.exec_cmd("hyprpm reload -n")
end)
```

### 📝 Legacy Config (`hyprland.conf`)
```ini
exec-once = hyprpm reload -n
```

---

## ⚙️ Easy Configuration Guide

Copy and paste this config snippet into your Hyprland configuration file to customize the look!

### 📜 Lua Configuration (`module/myglass.lua` / `hyprland.lua`)

```lua
local state_file = (os.getenv("HOME") or "/home/capture") .. "/.config/hypr/myglass_enabled.state"
local f = io.open(state_file, "r")
local enabled_state = true
if f then
    local content = f:read("*all")
    f:close()
    if content and content:find("false") then
        enabled_state = false
    end
end

if hl.plugin and hl.plugin.myglass then
    local hg = hl.plugin.myglass

    -- Main Settings
    hg.config({
        enabled = enabled_state,
        default_theme = "dark",      -- "dark" or "light"
        default_preset = "clear",     -- Glass style
        tint_color = 0x00000000,      -- Clear glass tint
        glass_opacity = 0.09,         -- Transparency opacity
        blur_strength = 0.04,         -- Background blur strength
    })

    if enabled_state then
        -- Add liquid glass effect to Dynamic Island layer surfaces
        hg.layer("nowoward-capdynamic", { preset = "clear" })
        hg.layer("nowoward-capdynamic-wallpaperpicker", { preset = "clear" })
        hg.layer("nowoward-capdynamic-wlogout", { preset = "clear" })

        -- Add liquid glass effect to Waybar & SwayNC
        hg.layer("waybar", { preset = "subtle" })
        hg.layer("swaync")
    end
end
```

### 📝 Legacy Config (`hyprland.conf`)

```ini
plugin:myglass {
    default_theme = dark
    default_preset = clear
    tint_color = 0x00000000
    glass_opacity = 0.09
    blur_strength = 0.04

    layers {
        enabled = 1
        namespaces = waybar, swaync, nowoward-capdynamic, nowoward-capdynamic-wallpaperpicker, nowoward-capdynamic-wlogout
        preset = clear
    }
}
```

### 💡 Neovim, btop & Terminal Setup

To render liquid glass seamlessly behind **Neovim**, **btop**, or terminal applications:

1. Enable background opacity in your terminal (e.g., `background_opacity 0.75` in `~/.config/kitty/kitty.conf`).

2. **btop System Monitor Glass Setup**:
   Disable `theme_background` in `~/.config/btop/btop.conf` so `btop` does not render a solid background:
   ```ini
   color_theme = "tokyo-night"  # or high-contrast theme
   theme_background = false
   truecolor = true
   ```
   *Tip for high text contrast over transparent wallpaper*: Create a theme file `~/.config/btop/themes/high-contrast-glass.theme` with `#ffffff` foreground colors for ultra-sharp text readability.

3. **Neovim Setup**:
   Add this snippet to your Neovim config (`~/.config/nvim/init.lua`) to make Neovim's background transparent:

```lua
-- Force transparent background in Neovim for MyGlass
for _, group in ipairs({ "Normal", "NormalNC", "LineNr", "SignColumn", "NormalFloat" }) do
    vim.api.nvim_set_hl(0, group, { bg = "NONE", ctermbg = "NONE" })
end
```

---

## 🛠️ Manual Building (For Advanced Users)

If you don't want to use `hyprpm`, you can compile directly from source code:

```bash
# 1. Download the code
git clone https://github.com/Sidharth7082/myglass.git
cd myglass

# 2. Build the plugin
make -j$(nproc)

# 3. Load the plugin into Hyprland
hyprctl plugin load $(pwd)/myglass.so
```

---

## ❓ Frequently Asked Questions & Fixes

<details>
<summary><b>1. The glass effect isn't showing up?</b></summary>

Make sure your windows are slightly transparent or translucent! You can also run:
```bash
hyprpm update && hyprctl reload
```
</details>

<details>
<summary><b>2. How do I completely uninstall MyGlass?</b></summary>

Run these two commands in your terminal:
```bash
hyprpm disable myglass
hyprpm remove myglass
```
</details>

<details>
<summary><b>3. What are the system requirements?</b></summary>

- Any Arch/Linux distro running **Hyprland**
- **GCC** or **Clang** compiler
- `hyprpm` (included automatically with Hyprland)
</details>

---

## 📜 License & Credits

- Created by **[Sidharth7082](https://github.com/Sidharth7082)** (Capture)
- Free and Open Source under the **[BSD 3-Clause License](LICENSE)**
