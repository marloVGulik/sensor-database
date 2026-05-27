# ============================================================
# CMake build configuration for database_sensor
# ============================================================

cmake_minimum_required(VERSION 3.10)
project(database_sensor VERSION 1.0)

# Export compile_commands.json for IDE tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS "on")

# C++11 required for std::stringstream and sqlite3 C API compatibility
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Release build if no build type specified
# Provides optimization flags (-O2) without debug symbols
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()

# Add vendored imtui library (Dear ImGui terminal UI) as subdirectory
# This brings in imgui, imtui core, and ncurses backend targets
add_subdirectory(vendor/imtui)

# Find system SQLite3 library and headers
find_package(SQLite3 REQUIRED)

# All source files for the application
set(SOURCES
    src/main.cpp
    src/database.cpp
    src/ui.cpp
    src/views/render_sensors.cpp
    src/views/render_values.cpp
    src/views/render_add.cpp
    src/views/render_update.cpp
    src/views/render_delete.cpp
)

# Header files (for IDE indexing; not compiled directly)
set(HEADERS
    src/models.h
    src/database.h
    src/ui.h
)

# Create executable from sources
add_executable(database_sensor ${SOURCES} ${HEADERS})

# Include paths for headers: project src, SQLite3, and vendored imtui
target_include_directories(database_sensor PRIVATE
    src
    ${SQLite3_INCLUDE_DIRS}
    vendor/imtui/include
    vendor/imtui/third-party
    vendor/imtui/src
)

# Link against imtui ncurses backend and SQLite3
target_link_libraries(database_sensor PRIVATE
    imtui-ncurses
    SQLite3::SQLite3
)

# Copy SQL schema and seed files to build directory so they're accessible at runtime
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/setup.sql DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/seed.sql DESTINATION ${CMAKE_BINARY_DIR})
