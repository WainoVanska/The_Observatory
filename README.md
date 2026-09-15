# The Observatory

The Observatory Scene is a real-time C++ and OpenGL learning project that combines the rendering techniques I studied into one interactive scene.

<img src="https://github.com/user-attachments/assets/e302b108-f384-4bd5-9276-2b2b9fb43b94" alt="-" width="700">

https://github.com/user-attachments/assets/e302b108-f384-4bd5-9276-2b2b9fb43b94

## Features

* Written in C++ using OpenGL 3.3 and GLSL.
* Free-moving first-person camera.
* Blinn–Phong lighting with multiple point lights and a flashlight.
* Omnidirectional point shadows using depth cubemaps.
* Percentage-closer filtering for smooth shadow edges.
* Cached shadow maps for stationary lights.
* Normal mapping and parallax occlusion mapping on the brick surfaces.
* HDR rendering, gamma correction and bloom.
* Model loading through Assimp.
* Instanced asteroid rendering.
* Space skybox using a cubemap.
* Stencil outlines and transparent model materials.
* Multisample antialiasing.
* Normal and framebuffer-depth visualization modes.
* In-engine video recording through FFmpeg.

### Notes

* Blue light only has an impartial implementation (no lighting on the walls and no shadows from objects) and should be treated as such.
* Normal debug mode doesn't contain the planet or asteroids.
* When the backpack material is toggled to glass (H key) the material only takes the skybox texture into consideration. The stars reflecting on the backpack look messy, this isn't intended because the original implementation was made with a different skybox texture initially.
* Video recording requires FFmpeg to be installed and available through the system `PATH`. On Windows, it can be installed with `winget install --id Gyan.FFmpeg --exact`.
* Recordings are saved in the application's working directory as `observatory_1.mp4`, `observatory_2.mp4`, and so on. Audio isn't recorded in the current setup.
* Window resizing has to be manually configured through `main.cpp` (WINDOW WIDTH / WINDOW HEIGHT values).

### Controls

| Key | Action |
|---|---|
| **W, A, S, D** | Move the camera |
| **Mouse** | Look around |
| **Space / Left CTRL** | Move up / down |
| **1** | Switch between stationary lights and movable light |
| **I, J, K, L** | Move the light position (after toggling key 1) |
| **U / O** | Move the light up / down |
| **2** | Toggle blue light (lighting only working on certain objects) |
| **F** | Toggle the flashlight |
| **N** | Toggle normal visualization |
| **V** | Toggle depth visualization |
| **Q / E** | Decrease / increase the parallax height scale |
| **M** | Toggle indoor/outdoor mode |
| **H** | Toggle the backpack model material |
| **P** | Toggle the backpack model outline |
| **B** | Toggle framebuffer blur effect |
| **R** | Start / stop video recording |
| **Esc** | Close the application |

## Learning material

| Material | Status |
|---|---|
| Fundamentals of Computer Graphics, Fifth Edition | 🟡 |
| Ray Tracing in One Weekend Book Series | 🔴 |
| [CMU Computer Graphics Lectures](https://youtube.com/playlist?list=PL9_jI1bdZmz2emSh0UQ5iOdT2xRHFHL7E&si=knAZi4WGLnnt8G_z) | 🟢 |
| [LearnOpenGL](https://learnopengl.com/) | 🟢 |

## Weekly progress

The repository documents my weekly graphics-programming progress, including implementations, problems, debugging and my thoughts on each learning stage.

## Topics in need of review

* Multipass rendering
* Geometry shaders
* G-buffers
* Instanced rendering
* Gamma correction
* Bloom
* Shadow-map bias and filtering

## How to build

1. Clone the repository.
2. Open `first_graphics_thing/first_graphics_thing.vcxproj` in Visual Studio.
3. Select an x64 Debug or Release configuration.
4. Build and run the project by pressing the F5 key.