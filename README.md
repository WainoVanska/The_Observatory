# The Observatory

The Observatory Scene is a real-time C++ and OpenGL learning project that combines the rendering techniques I studied into one interactive scene.

https://github.com/user-attachments/assets/b5647946-5dc7-4940-89ed-c57e5c343851

## Features

* C++ using OpenGL 3.3 and GLSL.
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
* Cinematic camera paths that are rendered to video frame by frame.

### Controls

| Key | Action |
|---|---|
| **W, A, S, D** | Move the camera |
| **Left Shift** | Move the camera slowly (hold) |
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
| **G** | Toggle the extra glass surfaces |
| **R** | Start / stop video recording |
| **[ / ], C, Backspace, T** | Select shot, add / remove keyframe, play and record |
| **Esc** | Close the application |

## Learning material

* Fundamentals of Computer Graphics, Fifth Edition
* [CMU Computer Graphics Lectures](https://youtube.com/playlist?list=PL9_jI1bdZmz2emSh0UQ5iOdT2xRHFHL7E&si=knAZi4WGLnnt8G_z)
* [LearnOpenGL](https://learnopengl.com/)

## Weekly progress

The repository documents my weekly graphics-programming progress, including implementations, problems, debugging and my thoughts on each learning stage.

[View the weekly learning log](docs/learning-log/)

### Notes

* Blue light only has a partial implementation (no lighting on the walls and no shadows from objects) and should be treated as such.
* Normal debug mode doesn't contain the planet or asteroids.
* When the backpack material is toggled to glass (H key) the material only takes the skybox texture into consideration. The stars reflecting on the backpack look messy, this isn't intended because the original implementation was made with a different skybox texture initially.
* Video recording requires FFmpeg to be installed and available through the system `PATH`. On Windows, it can be installed with `winget install --id Gyan.FFmpeg --exact`.
* Recordings are saved in the `recordings` folder of the application's working directory as `observatory_1.mp4`, `observatory_2.mp4`, and so on. Audio isn't recorded in the current setup.
* Window resizing has to be manually configured through `main.cpp` (`WINDOW_WIDTH` / `WINDOW_HEIGHT` values).

## How to build

### Requirements

* Windows 10 or 11 and an OpenGL 3.3-capable GPU.
* Visual Studio with the **Desktop development with C++** workload.
* Git.

### Set up vcpkg

Run in PowerShell:

```powershell
git clone https://github.com/microsoft/vcpkg.git "$env:USERPROFILE\vcpkg"
& "$env:USERPROFILE\vcpkg\bootstrap-vcpkg.bat"
& "$env:USERPROFILE\vcpkg\vcpkg.exe" integrate install
```

### Build

1. Clone this repository and open `The_Observatory.slnx` in Visual Studio.
2. Select **x64 Debug** or **x64 Release**.
3. Press **F5** to build and run.

## Credits

* **Backpack model:** by Berk Gedik ([Sketchfab](https://sketchfab.com/3d-models/survival-guitar-backpack-low-poly-799f8c4511f84fab8c3f12887f7e6b36)), with materials adapted by Joey de Vries for LearnOpenGL.
* **Planet model:** by Gerhald3D ([TurboSquid](https://www.turbosquid.com/3d-models/realistic-mars-photorealistic-2k-3d-1277433)), modified by Joey de Vries for LearnOpenGL.
* **Rock model, brick wall textures and container textures:** from the [LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL) resources by Joey de Vries.
* **Brick displacement map:** made by me from the brick wall texture.
* **Observatory window texture:** AI-generated.
* **Space skybox, plaster and wood floor textures:** free assets downloaded during development; their original sources weren't recorded.
