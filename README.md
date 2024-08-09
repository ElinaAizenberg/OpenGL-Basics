# OpenGL: object loader, camera system
## Overview
This educational mini-application is designed to demonstrate essential object manipulation techniques and UI interaction within a 3D environment using OpenGL. The application provides users with the ability to create, modify, and manage multiple objects in a scene, offering a hands-on approach to understanding basic graphics programming concepts. Through its intuitive interface, users can experiment with object transformations, panel controls, and real-time feedback, making it an ideal learning tool for those exploring 3D graphics and interactive application development. The code is heavily commented, with detailed function descriptions to facilitate learning and ease of understanding.

## Features
- **Object Management:**
  - easily add, remove, and modify objects within the scene using a dedicated panel;
  - customize object properties such as color and position, with options to reset or select specific objects.

- **Interactive Settings:**
  - adjust global settings like rotation sensitivity and metadata display;
  - lock individual panels to objects for synchronized movement and interaction.
  
- **Selection and Manipulation:**
  - select and manipulate objects using various methods, including individual, area, and batch selection;
  - move and rotate objects directly within the scene using intuitive mouse controls.

- **Dynamic Panels:**
  - open, close, and move individual object panels independently or lock them to the objects for consistent tracking;
  - customize object panels with metadata, color modes, zoom controls, and comments.

- **Zoom Functionality:**
  - easily zoom in and out of the scene to get a closer look at the objects or view the entire scene at once.


## Screenshots
<img src="https://github.com/user-attachments/assets/6a013280-a3b0-4abd-9e5d-1219e2f31e88" width="700">

<img src="https://github.com/user-attachments/assets/6b2a2458-5eef-4fa9-b148-2aec799400bb" width="700">

## Getting Started
### Prerequisites
- Compiler that supports C++
- CMake (>= 3.10)
- OpenGL (version 3.0)
- GLFW
- GLEW

**External libraries:**
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImGui Logger](https://github.com/leiradel/ImGuiAl/tree/master/term)

### Installation
1. Clone the repo
```
git clone https://github.com/ElinaAizenberg/OpenGL-Basics
cd OpenGL-Basics
```

2. In CMakeLists.txt and logger/CMakeLists.txt update the path to the folder with external libraries
```
#TODO Specify the directory where imgui and imguial_term libraries are located
set(EXTERNAL_LIB_DIR /path_to_directory/libs)
```

3. Build project
```
mkdir build
cd build
cmake ..
make
```

4. Run project
```
./project_1
```

