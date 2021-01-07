cmake_minimum_required(VERSION 3.13)
project(mcprofiles VERSION 0.3 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Thanks stack https://stackoverflow.com/a/61381440/10821333
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall)
endif()

file(GLOB_RECURSE sources src/*.cpp)

set(NBT_USE_ZLIB OFF)
set(NBT_BUILD_TESTS OFF)
set(NBT_BUILD_SHARED OFF)
add_subdirectory(deps/libnbtplusplus)

add_executable(mcprofiles ${sources})

set_target_properties(mcprofiles PROPERTIES OUTPUT_NAME "mcp")

target_include_directories(mcprofiles
    PUBLIC "deps/include"
)

target_link_libraries(mcprofiles
    stdc++fs
    nbt++
)

