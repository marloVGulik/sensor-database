cmake_minimum_required(VERSION 3.10)
project(SensorDatabaseProgram)

set(CMAKE_CXX_STANDARD 17)

# --- Fix for the CMP0072 Warning ---
set(OpenGL_GL_PREFERENCE GLVND)

# Find necessary system packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
)

# ImGui Source files
set(IMGUI_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

# Your application source files
file(GLOB_RECURSE APP_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Create executable
add_executable(${PROJECT_NAME} ${APP_SOURCES} ${IMGUI_SOURCES})

# Link libraries (SQLite3, GLFW, OpenGL)
target_link_libraries(${PROJECT_NAME}
    glfw
    OpenGL::GL
    sqlite3
    dl
    pthread
)