# Scientific Field Visualizer

A real-time GPU-accelerated scientific visualization tool built with C++ and OpenGL. 
Renders scalar and vector fields over procedural 3D meshes generated via Marching Cubes.

![Demo GIF](media/demo.gif)

---

## Features

### Core
- [x] Procedural mesh generation via Marching Cubes (CPU + GPU compute shader)
- [x] Per-vertex scalar field visualization with analytical colormap implementation
- [x] Orbital camera — left drag to orbit, right drag to pan, scroll to zoom
- [x] Lambertian diffuse shading with per-vertex normals

### Level 2
- [x] Colormap selector — Viridis, Jet, Turbo (analytical, no texture lookups)
- [x] Wireframe overlay toggle — barycentric coordinate method, no second draw call
- [x] Scalar range control — interactive min/max clamping with out-of-range highlight

### Level 3
- [x] Displacement / deformation overlay — per-vertex vector field, GPU deformation in vertex shader
- [x] Isoline rendering — fragment shader contour lines using fwidth for screen-consistent width
- [x] GPU Compute Shader — Marching Cubes runs entirely on GPU via compute shader,
      output written directly to SSBO bound as VBO (zero CPU readback)

---

## Mesh Library

| Shape | Description |
|-------|-------------|
| Torus | Classic genus-1 surface |
| Gyroid | Triply periodic minimal surface |
| Schwarz P | Cubic periodic minimal surface |
| Double Torus | Union of two interlocked tori |
| Sphere with Holes | Sphere minus 3 axis-aligned cylinders |
| Diamond | Diamond cubic TPMS |
| Trefoil Knot | Knotted surface |

## Scalar Field Library

| Field | Description |
|-------|-------------|
| Sine Ripples | Radial waves from center |
| Product Bands | XYZ sine product |
| Twisted Bands | Spiral bands around Y axis |
| Concentric Shells | Distance-based shells |
| Diagonal Gradient | Turbulent diagonal gradient |
| Polar Angle | Angular field around Y axis |
| Checkerboard | 3D checkerboard pattern |
| Displacement Magnitude | Visualizes displacement field strength |

---

## Build & Run

### Quick Start (Windows)
A precompiled `.exe` is available in [Releases](https://github.com/AmoghJ/your-repo/releases).  
Download and run — no installation required.

### Build from Source

**Prerequisites:**
- Visual Studio 2022
- [vcpkg](https://github.com/microsoft/vcpkg)

**Install dependencies via vcpkg:**
```bash
vcpkg install glm imgui[docking-experimental,glfw-binding,opengl3-binding] glew glfw3
```

**Build:**
1. Clone the repository
```bash
git clone https://github.com/AmoghJ/your-repo.git
cd your-repo
```
2. Open `ScientificVisualizer.sln` in Visual Studio 2022
3. Set vcpkg toolchain in project properties if not auto-detected
4. Build → Release x64
5. Run

**Controls:**

| Input | Action |
|-------|--------|
| Left drag | Orbit camera |
| Right drag | Pan camera |
| Scroll | Zoom |
| W | Toggle wireframe |
| I | Toggle isolines |

---

## Architecture

The renderer uses a component-based architecture with an event/messaging system 
for decoupled communication between modules.

**Key systems:**
- `MarchingCubes` — procedural mesh generation, CPU and GPU paths
- `OpenGLViewer` — FBO-based render-to-texture, displayed via ImGui
- `OrbitCamera` — spherical coordinate camera with MVP rebuild on viewport resize
- Compute shader pipeline — SSBO output bound directly as VBO, zero CPU readback

---

## LLM Usage

---

## What I Would Do Differently

---

## Dependencies

- [OpenGL 3.3+](https://www.opengl.org/)
- [GLFW](https://www.glfw.org/)
- [GLEW](http://glew.sourceforge.net/)
- [GLM](https://github.com/g-truc/glm)
- [Dear ImGui](https://github.com/ocornut/imgui) (docking branch)