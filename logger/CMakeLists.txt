# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.0)

project(logger_library)

# Create a static library target
add_library(logger_library STATIC logger.cpp)

#TODO Specify the directory where imgui and imguial_term libraries are located
set(EXTERNAL_LIB_DIR /path_to_directory/libs)

# Include the directory where the external library headers are located
include_directories(${EXTERNAL_LIB_DIR}/imgui)
include_directories(${EXTERNAL_LIB_DIR}/imguial_term)

# Add the external library source files to the static library
target_sources(logger_library PRIVATE ${EXTERNAL_LIB_DIR}/imguial_term/imguial_term.cpp)
target_sources(logger_library PRIVATE ${EXTERNAL_LIB_DIR}/imgui/imgui.cpp)

# Specify the lib directory that should be included in the build process for the logger_library target
target_include_directories(logger_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
