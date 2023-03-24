cmake_minimum_required(VERSION 3.15)
project(crga)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# Dependencies
set(RAYLIB_VERSION 4.2.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

# The Project
if (UNIX)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/DEBUG)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/LIBRARY)
endif()
set(SOURCES src/crga.c src/crga.h src/crgahelper.h src/termdraw.h)
add_executable(${PROJECT_NAME} src/main.c ${SOURCES})
#add_library(${PROJECT_NAME} SHARED ${SOURCES})
#set(raylib_VERBOSE 1)
target_compile_features(${PROJECT_NAME} PUBLIC c_std_99)
target_link_libraries(${PROJECT_NAME} raylib)
if (WIN32)

endif()
if (UNIX)
  target_link_libraries(${PROJECT_NAME} ncurses)
endif()
#target_link_libraries(${PROJECT_NAME} notcurses-core)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
