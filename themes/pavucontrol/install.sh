#!/usr/bin/env bash
set -e

echo "🧊 Installing MyGlass Pavucontrol Smoked Glass Theme..."

# 1. Install GTK4 & GTK3 CSS
mkdir -p ~/.config/gtk-4.0 ~/.config/gtk-3.0
curl -fsSL https://raw.githubusercontent.com/Sidharth7082/myglass/main/themes/pavucontrol/gtk-4.0/gtk.css -o ~/.config/gtk-4.0/gtk.css
cp ~/.config/gtk-4.0/gtk.css ~/.config/gtk-3.0/gtk.css

# 2. Inject Hyprland Window Rule if not present
RULES_FILE="$HOME/.config/hypr/module/rules.lua"
if [ -f "$RULES_FILE" ] && ! grep -q "pavucontrol-glass-opacity" "$RULES_FILE"; then
    cat << 'RUEOF' >> "$RULES_FILE"

-- Apply glass transparency to Pavucontrol audio manager
hl.window_rule({
	name    = "pavucontrol-glass-opacity",
	match   = { class = "^(org.pulseaudio.pavucontrol|pavucontrol)$" },
	opacity = 0.70,
})
RUEOF
fi

# 3. Reload Hyprland
hyprctl reload >/dev/null 2>&1 || true

echo "✨ MyGlass Pavucontrol Smoked Glass Theme installed successfully! Launch 'pavucontrol' to view."
