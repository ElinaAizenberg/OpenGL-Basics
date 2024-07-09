# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)
# Project name
project(project_1)

set(CMAKE_CXX_STANDARD 14)

# Set policy to suppress the OpenGL warning
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 OLD)
endif()

#TODO Specify the directory where imgui and imguial_term libraries are located
set(EXTERNAL_LIB_DIR /path_to_directory/libs)

# Define the path to the central ImGui directory
set(IMGUI_DIR ${EXTERNAL_LIB_DIR}/imgui)

# Add the subdirectory containing the internal library
add_subdirectory(logger)

# Add source files from the project
set(PROJECT_SRC
        src/drawing_lib.cpp
        src/main.cpp
        src/gui_panels.cpp
        src/object.cpp
        src/session.cpp
        src/font.cpp
)

# Add ImGui source files
set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add the include directory to the include path
include_directories(include)
# Include the directory where the external library headers are located
include_directories(${IMGUI_DIR} ${EXTERNAL_LIB_DIR}/imguial_term ${IMGUI_DIR}/backends)

configure_file(ReadMe.txt include/ReadMe.txt COPYONLY)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED CONFIG)
find_package(GLEW REQUIRED)

# Add executable
add_executable(${PROJECT_NAME} ${PROJECT_SRC} ${IMGUI_SRC})

# Link libraries
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw GLEW::GLEW dl)
target_link_libraries(${PROJECT_NAME} logger_library)
