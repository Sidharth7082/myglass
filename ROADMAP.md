# MyGlass Roadmap

## v1.1.0 (Released)
- [x] Full hyprpm support
- [x] Lua integration (`hl.plugin.myglass`)
- [x] Dynamic island layer glass support
- [x] btop system monitor glass documentation & high-contrast theme

## v1.2.0 (Released)
- [x] Built-in `terminal_glass` & `acrylic` presets
- [ ] Shared framebuffer pool for reduced GPU memory latency
- [ ] Uniform location caching in ShaderManager
- [ ] Live state file hot-reloading

## v1.3.0 (Released)
- [x] Duplicate glass decoration guard (fixes double render passes on window remap)
- [x] Crash-proof config reload (v1.4.0)
- [ ] Per-window blur strength customization
- [ ] Glass dynamic edge glint animations

## v1.4.0 (Released)
- [x] Fix SIGABRT crash on config reload (exception-safe init & reload paths)
- [x] Correct plugin version reporting (`hyprctl plugin list` now shows 1.4.0)
- [x] Hyprland 0.56.1 commit pin for deterministic hyprpm builds
- [x] Render-thread exception hardening (no throws from shader loading)
- [x] Null-safety + degenerate-blit hardening in the GL pipeline

## v1.5.0 (Released)
- [x] Blur kernel scales with `blur_strength` instead of clamping to 8 px
- [x] Blur cache invalidates when strength/iterations/downscale change
- [x] Window pass boxes in monitor-local logical coordinates (HiDPI correctness)
- [x] No `RASSERT` abort when a live-blur element has no bounding box
- [x] Layer compositing skips stale samples after a failed redirect
- [x] `sampleBackground()` restores the caller's framebuffer
- [x] One-shot shader compilation (no per-frame retry spam)
- [x] `edge_thickness = 0` no longer generates NaN
- [x] Global `enabled` gates layer glass
- [x] Makefile header dependency tracking + compiler detection
- [x] Hyprland 0.56.2 commit pin for deterministic hyprpm builds
- [ ] Shared framebuffer pool for reduced GPU memory latency
- [ ] Uniform location caching in ShaderManager
- [ ] Live state file hot-reloading

