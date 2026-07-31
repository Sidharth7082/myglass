# MyGlass Roadmap — Performance Edition

> **Core Philosophy**: *"Same visuals, significantly lower GPU usage, lower RAM usage, and faster rendering."*

---

## 🎯 Target Metrics (v1.2.0 Benchmark Goals)

| Metric | v1.1.0 Baseline | v1.2.0 Target | Goal |
|---|---:|---:|---|
| **FPS** | 165 | **165+** | Rock-solid refresh rate stability |
| **Frame Time** | 6.0 ms | **4.0–5.0 ms** | 20–30% render latency reduction |
| **VRAM Usage** | ~120 MB | **< 90 MB** | Smart FBO pooling & allocation |
| **RAM Usage** | ~70 MB | **< 50 MB** | Object pooling & zero-alloc frame loops |
| **GPU Usage** | 100% | **60–75%** | Damage-based partial blurring & caching |
| **Blur Passes** | Every Frame | **Only Damaged** | Skip re-renders on static windows/layers |

---

## ⚡ v1.2.0 Performance Priorities

### 1. Damage-Based Rendering ⭐⭐⭐⭐⭐
- Only blur damaged framebuffer regions instead of the entire window area.
- Skip rendering for windows whose background and geometry have not changed.
- Expected gain: **20–50% GPU load reduction**.

### 2. Smart Framebuffer Reuse (Framebuffer Pool) ⭐⭐⭐⭐⭐
- Implement a global `FramebufferPool` to eliminate per-frame `createFB()` / `alloc()` calls.
- Reuse framebuffers matching identical width, height, and DRM formats.
- Expected gain: **Reduced driver stalls & zero memory fragmentation**.

### 3. Blur Caching ⭐⭐⭐⭐⭐
- Retain the blurred background texture across frames.
- Invalidate cache only when window position moves, monitor changes, or underlying damage occurs.

### 4. Partial & Visible Region Blur ⭐⭐⭐⭐☆
- Restrict blur pipeline execution strictly to visible/unobscured window regions.
- Huge performance gain for large overlapping terminals and tiled layouts.

### 5. Occlusion Culling ⭐⭐⭐⭐⭐
- Detect fully covered windows in the stack and skip glass rendering completely.

### 6. Shader Optimization ⭐⭐⭐⭐⭐
- Optimize Gaussian blur fragments to minimize texture fetches and eliminate conditional branching.
- Precompute scale and direction uniforms on CPU.

### 7. Window Size Thresholding ⭐⭐⭐⭐☆
- Apply lightweight single-pass shaders for tiny windows, popups, and tooltips.

### 8. Uniform Upload Caching (Dirty Presets) ⭐⭐⭐⭐☆
- Only call `glUniform*` when active presets or resolved theme parameters actually change.

### 9. OpenGL State Sorting ⭐⭐⭐⭐⭐
- Batch and sort render draw calls by preset and shader state to minimize GL state transitions.

### 10. SIMD Vectorized Geometry Math ⭐⭐⭐⭐☆
- Use SIMD / compiler vectorization for bounding box intersections and coordinate transformations.

### 11. Object & Region Memory Pools ⭐⭐⭐⭐⭐
- Replace allocations in rendering loops with reusable object pools for rectangles, vectors, and regions.

### 12. Skip Invisible Layers & Offscreen Windows ⭐⭐⭐⭐⭐
- Instantly bypass surfaces with `opacity = 0`, hidden state, or offscreen viewports.

### 13. Per-Monitor Independent Damage Updates ⭐⭐⭐⭐☆
- Restrict scene damage invalidation strictly to the monitor where events occurred.

### 14. Region Union Caching ⭐⭐⭐⭐⭐
- Avoid redundant polygon/region merging per frame by caching region unions.

### 15. Lazy Evaluation ⭐⭐⭐⭐⭐
- Defer preset, blur radius, tint, and opacity calculations until explicitly requested during render passes.

---

## 🔬 Profiling & Verification Methodology

Before and during v1.2.0 implementation, all optimizations will be empirically validated using profiling tools:
- **GPU Profiling**: `apitrace` and RenderDoc to trace driver draw calls, texture allocation, and shader pass execution times.
- **CPU & Allocation Profiling**: `perf` and `valgrind --tool=callgrind` to track memory allocations and CPU math hot spots.

---

## 🔮 Future Enhancements (v1.3+)

- [ ] Acrylic / noise grain shader overlays
- [ ] Per-window custom blur strength sliders
- [ ] Smooth glass transition animations on focus change
