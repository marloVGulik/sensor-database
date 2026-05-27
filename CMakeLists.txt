cmake_minimum_required(VERSION 3.10)
project(database_sensor VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS "on")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type default
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()

# Add imtui as subdirectory
add_subdirectory(vendor/imtui)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/database.cpp
    src/ui.cpp
)

set(HEADERS
    src/models.h
    src/database.h
    src/ui.h
)

# Executable
add_executable(database_sensor ${SOURCES} ${HEADERS})

target_include_directories(database_sensor PRIVATE
    src
    ${SQLite3_INCLUDE_DIRS}
    vendor/imtui/include
    vendor/imtui/third-party
    vendor/imtui/src
)

target_link_libraries(database_sensor PRIVATE
    imtui-ncurses
    SQLite3::SQLite3
)

# Copy SQL files to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/setup.sql DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/seed.sql DESTINATION ${CMAKE_BINARY_DIR})
