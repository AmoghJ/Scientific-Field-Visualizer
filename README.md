# Scientific Field Visualizer

A real-time GPU-accelerated scientific visualization tool built with C++ and OpenGL. 
Renders scalar and vector fields over procedural 3D meshes generated via Marching Cubes and Compute Shaders.
Also simulates GPU-accelerated particle advection in a vector field using compute shaders.

---

*The GIF may take some time to load:*
![Demo GIF](Media/demo.gif)

## Assignment 1 (Field Visualizer on Triangular Mesh) - Features

### Core
- [x] (Finished) Procedural geometry generation via Marching Cubes (CPU + GPU compute shader). I chose this approach as I thought it was a good opportunity to demonstrate compute shaders as well. The current architecture can easily allow to add a file importer and render without much change. The marching cube algorithm samples from a set of analytical functions (vec3->float) that describe various geometric objects at discrete points in 3D space. Then lookup tables are used to generate the triangles based on a certain isovalue for the geometric function. The resultant geometry is the isosurface of the function.
- [x] (Finished) Per-vertex scalar field visualization with analytical colormap implementation. The selected analytical function (vec3->float) is sampled at the vertex position and passed in as vertex attribute. The fragment shader then uses a transfer function to color it accordingly.
- [x] (Finished) Orbital camera — left drag to orbit, right drag to pan, scroll to zoom. The class keeps track of the current orientation of the camera and calculates a MVP matrix that is passed to the vertex shader. The MVP matrix is used to transform all vertices to simulate a virtual camera.
- [x] (Finished) Lambertian diffuse shading with per-vertex normals. Smooth normals would have been ideal but has been skipped for now in interest of time. Smooth normals would require a mesh data structure with adjacency information to be able to interpolate the normals at each vertex. Right now, dragging the cell size down on the marching cube generator makes a much dense mesh and effectively even with flat normals gives smooth shading (albeit at higher computational cost)

### Level 2
- [x] (Finished) Colormap selector — Viridis, Jet, Plasma, etc (analytical, no texture lookups, all calculated directly in the fragment shader). The appropriate colormap (float->vec3) is selected based on a uniform that represents the index for the function. Unfortunately we can't pass C style function to the GPU from the CPU and have function pointers in GLSL. So the approach of having if statements seems like the only viable option (if one can't use texturemaps)
- [x] (Finished) Wireframe overlay toggle — barycentric coordinate method, no second draw call. The barycentric coordinates are evaluated in the vertex shader. After interpolation into the fragment shader, we get screen space fragment information that tells us how close a particular pixel is to the triangle edge. This is then used to draw a line with the step function.
- [x] (Finished) Scalar range control — interactive min/max clamping with out-of-range clamped to colormap extremes. The interpolated parameter from the vertex shader is simple remapped to the new min/max bounds and used with the transfer function. This has the effect of bounding the colors to a certain range of the parameter and the colors remain the same on the extreme side of both the bounds. To have a different color at the extremes, we could use an if statement to check if the parameter is out of bounds and color accordingly.

### Level 3
- [x] (Finished) Displacement / deformation overlay — per-vertex vector field, GPU deformation in vertex shader. Another set of analytical functions (vec3->vec3) are defined and passed in as vertex attributes. The vertices are then deformed in world space (before applying MVP). The normals need to be recalculated after the deformation, so a geometry shader is added to the pipeline to calculate the normals based on the deformed vertices.
- [x] (Finished) Isoline rendering — fragment shader contour lines using fwidth for screen-consistent width. A step function is used to draw a line in the fragment shader for wherever the parameter equals a certain value. The line is scaled using fwidth to keep the thickness constant even if we zoom in or out.
- [x] (Finished) GPU Compute Shader — Marching Cubes runs entirely on GPU via compute shader, output written directly to SSBO bound as VBO (zero CPU readback). Marching cubes already lends itself nicely to compute shader since all the cells are calculated independently. The compute shader also calculates the normals, scalar field, deformation field for each vertex. It essentially prepares all the buffers for the vertex shader. Another advantage is that the SSBO don't need CPU-GPU transfers. The vertex shaders directly read the SSBOs as VBOs.

---

*The GIF may take some time to load:*
![Demo2 GIF](Media/demo2.gif)

## Assignment 2 (GPU Particle Advection in Vector Field) - Features

### Core
- [x] (Finished) A 3D vector field (vec3->vec3) is defined analytically in the compute shader. It uses the curl function to get natural looking vortices. There is random variation for each component and with respect to time to get interesting behaviour.
- [x] (Finished) The compute shader works on SSBO buffers (entirely on GPU) to advect the particles using RK4 step. There are 3 separate buffers for position, velocity, and age that are update continously once the simulation is running. The age is also advanced each cycle. (Currently simulating 120,000 trails - each with 128 vertices = 15,360,000 vertices). There is an option to reset the advection such that it respawns all the points randomly. The advection can be paused and played as well - done by controlling the time variable.
- [x] (Finished) Particles are rendered using a separate velocity field (vec3->vec3, for more visual interest) and age. The fragment shader uses the age to control the alpha and there is a simple mix function that is interpolated based on the velocity field.
- [x] (Finished) Particles reset if they leave the bounding box or live longer than their age.

### Level 2
- [x] (Finished) Trailines are implemented by adding a continous buffer that stores all the vertex positions for all the trails. Every run of the simulation progresses the vertices ahead by shifting within the array and writing a new position at the end of the array (within the bounds of each trail in the array). This is a little less performant as it involves shifting the array but was done in interest of time. Another approach could be to have another buffer that works as a pointer for each trailine within the continous array. The pointer points to the position from where the trail starts. This can then be read by the vertex shader and use it to render the trailine accordingly.
- [x] (Finished) Time varying field was added by simply introducing time variable in the parameter of the curl function for the analytical field.

### Level 3 (Not Implemented)
(Not Finished) Ping-Pong Buffer : Can be implemented by having two buffers (one for last state, and one for current state). The compute shader can only read from last state buffer and only write to the current state buffer. Each simulation cycle, the pointers for the two buffers are swapped. This reduces the chance of reading and writing to the same buffer position from different threads - however right now the compute shader program is carefully written to avoid this.

(Not Finished) Trilinear Interpolation for Field : First the field can be defined on the CPU side and uploaded as a GL_TEXTURE_3D to the compute shader. Then the field can be sampled as is already being done using the particle positions. We get trilinear sampling for free on the gpu. Currently this is not implemented as time-varying the field would become inefficient as it would require updated the field on the CPU and uploading it to the gpu on every call.

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

---

## Architecture

The renderer uses a component-based architecture with an event/messaging system 
for decoupled communication between modules.

Before starting a project, I usually start with a sketch and figure out how all the modules will be pieced together. Overall, I spent around an hour thinking about the architecture before beginning development for this project.

![Architecture Sketch](Media/ArchitectureSketch.jpg)

**Key systems:**
- `MarchingCubes`,`GPUAdvection` — procedural mesh generation, CPU and GPU paths, compute shaders
- `OpenGLViewer`, `AdvectionViewer` — FBO-based render-to-texture, displayed via ImGui
- `OrbitCamera` — spherical coordinate camera with MVP rebuild on viewport resize
- Compute shader pipeline — SSBO output bound directly as VBO, zero CPU readback

This architecture ensures that new features can be easily added without breaking anything. It makes it super easy to debug problems as they are local and there aren't much dependencies that are across the project. For e.g - it was really easy to add particle advection on top even though I hadn't planned for it initially.

---

## LLM Usage

I used Anthropic's Claude to have conversations and help in navigating some aspects of the projects. With chatbots, my workflow has changed from going to stackoverflow / or the documentation, to directly asking the chatbot. This has saved a lot of time browsing for the right function for the task. It helps tremendously in debugging as well since graphics programming has a lot of lines that require hardcoded values. A single incorrect digit and the whole program doesn't run without throwing an error. These are very difficult to catch, fortunately now chatbots are very effective at catching these.

It also helps to learn new concepts quickly. Eearlier I would have gone on forums to understanda topic - For e.g. I didn't know about the barycentric coordinates trick to render wireframe. But once the AI showed me, I immediately realised how easy and effective it is. What would have probably taken me an hour (most of the time just spent on searching the right information), now takes minutes (the concept is simple to understand).

In my experience, AI is very good at small context related tasks (I used it to generate the orbit camera class, functions for shape generators, scalar fields etc), but lack insight when it comes to large architecture decisions. In the beginning, I had a chat asking if in the context of the assignment, a MVC pattern with a container system would be good? Its reply was that it would be an overkill for the assignment. It thought that I would end up fighting the system rather than developing useful features. However, given the incremental nature of features to be implemented and the complexity of graphics related tasks, my experience told me that a robust architecture in the beginning saves a lot of time debugging and mass confusion later. I decided to stick with my architectural plan and it helped me a lot throughout the project.

**Overall, I think one needs a good idea about what the generic things are (such as camera orbiting, transfer functions etc) - where the AI probably has a lot of data that it is trained on, as opposed to specific things where the AI is more likely to misjudge and human judgement becomes necessary.**

---

## What I Would Do Differently

1. Smooth normals for marching cubes output would require building a vertex adjacency structure — mapping each unique position to all triangle faces that share it, then averaging their face normals. In interest of time I used face normals, which accurately represent the discrete mesh structure. A production implementation would use a hash map keyed on vertex position to accumulate and normalize shared normals in a post-process pass.

2. Grabbing input from ImGui is not ideal - right now the mouse input is being grabbed from openglviewer and sent to camera, the camera again sends updated info back. Ideally would implement a dedicated input handling system that is cross-platform.

3. Many hardcoded values for uniforms inside the shaders. A lot more values can be exposed through the GUI, but intentionally kept it lean for the assignment.

4. IModelData inheriting from component - this is basically so that each model can notify the container that it is done loading the model. I would have implemented a callback within modeldata to signal syncing of geometry.

5. Small optimization - esp. for push_back into std::vector -> too many copies and allocations. Ideally would want one allocation and no copies when generating geometry on cpu. This was partly ommitted by using compute shaders that directly allocate memory on GPU.

6. Anti-aliasing features to better display isolines and wireframes.

7. Normal coordinate space when applying transformations to mesh (currently they are in object space), I skipped this for now since mesh is not transforming. Ideally would setup a scene hierarchy structure with the ability to have parent-child relationship. Internally, the transformations matrices would be applied accordingly and the normals will be transformed to world space.

8. Need better way of sending vbo pointers to model class from render class. I modified the component, container event handling to have non-const value but this is not ideal as it may break things elsewhere.

9. OpenGLViewer and AdvectionViewer are largely the same classes. I hadn't planned on implementing advection earlier. But there should be an abstract class for the common functionality, and then the derived specialized classes for these viewers (Just like the model). Right now, I have just copy pasted code in interest of time.

---

## Dependencies

- [OpenGL 4.3+](https://www.opengl.org/)
- [GLFW](https://www.glfw.org/)
- [GLEW](http://glew.sourceforge.net/)
- [GLM](https://github.com/g-truc/glm)
- [Dear ImGui](https://github.com/ocornut/imgui) (docking branch)