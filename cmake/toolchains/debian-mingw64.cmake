# Build static to avoid unfortunate events
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(BUILD_SHARED_LIBS OFF)

# Ripped from GLFW FYI https://github.com/glfw/glfw/blob/master/CMake/x86_64-w64-mingw32.cmake

# Define the environment for cross-compiling with 64-bit MinGW-w64 GCC
set(CMAKE_SYSTEM_NAME    Windows) # Target system name
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_C_COMPILER     "x86_64-w64-mingw32-gcc")
set(CMAKE_CXX_COMPILER   "x86_64-w64-mingw32-g++")
set(CMAKE_RC_COMPILER    "x86_64-w64-mingw32-windres")
set(CMAKE_RANLIB         "x86_64-w64-mingw32-ranlib")

# Configure the behaviour of the find commands
set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

