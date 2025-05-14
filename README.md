# GwathTawar
A small, work-in-progress renderer using Vulkan. Right now, only Windows is supported.
The main motivation for this project is to learn new skills and techniques using C++ while creating something I’m passionate about. </br>
Take a look at [Core](https://github.com/schuetzda/GwathTawar/tree/main/GwaCore/src/Core) for the foundational components of the applications (Window, Camera, etc.). </br>
In [ecs](https://github.com/schuetzda/GwathTawar/tree/main/GwaCore/src/ecs) is a small entity-component-system using a Sparse Set at its core. </br>
[Renderer](https://github.com/schuetzda/GwathTawar/tree/main/GwaCore/src/renderer) includes all components responsible for rendering the 3D image to the screen. </br>

Additional used libraries are:
- [GLFW](https://github.com/glfw/glfw) for creating windows, contexts and surfaces, receiving input and events.
- [premake](https://premake.github.io/) to generate build configurations for Visual Studio 2022
- [gltf](https://github.com/jkuhlmann/cgltf) to load three-dimensional scenes and models using the [gltf data format](https://www.khronos.org/gltf/)
- [stb_image](https://github.com/nothings/stb) for reading and writing images
- [glm](https://github.com/g-truc/glm) is used as the math library
- [imgui](https://github.com/ocornut/imgui) for the graphical user interface

Install the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/). **Important** Make sure to select 'Shader Toolchain Debug Symbols' in the installation dialog, and ensure that your graphics card driver is up to date.

Then clone recursively: git clone --recurse-submodule https://github.com/schuetzda/GwathTawar
and run BuildScript.bat to build the project.
![](https://github.com/schuetzda/GwathTawar/blob/main/screenshots/Screenshot%202025-03-26%20144801.png)
