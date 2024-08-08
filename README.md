# OpenGL: object loader, camera system
## Overview

## Features


## Screenshots
<img src="https://github.com/user-attachments/assets/6a013280-a3b0-4abd-9e5d-1219e2f31e88" width="500">

<img src="https://github.com/user-attachments/assets/6b2a2458-5eef-4fa9-b148-2aec799400bb" width="500">

## Getting Started
### Prerequisites
- Compiler that supports C++
- CMake (>= 3.10)
- OpenGL (version 3.0)
- GLFW
- GLEW

**External libraries:**
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImGui Portable File Dialogs](https://github.com/samhocevar/portable-file-dialogs)
- [Image writer stb](https://github.com/nothings/stb/blob/master/stb_image_write.h)


### Installation
1. Clone the repo
```
git clone https://github.com/ElinaAizenberg/OpenGL-Basics
cd OpenGL-Basics
```

2. In CMakeLists.txt update the path to the folder with external libraries
```
#TODO Specify the directory where imgui and other libraries are located
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

