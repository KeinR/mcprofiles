cmake_minimum_required(VERSION 3.13)
project(mcprofiles VERSION 1.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Thanks stack https://stackoverflow.com/a/61381440/10821333
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall)
endif()

set(sources
    src/main.cpp
    src/Profiles.cpp
    src/util.cpp
    deps/gzstream/gzstream.cpp
)

find_package(ZLIB REQUIRED)

add_executable(mcprofiles ${sources})

set_target_properties(mcprofiles PROPERTIES OUTPUT_NAME "mcp")
target_compile_definitions(mcprofiles PUBLIC GZSTREAM_NAMESPACE=gz)

target_include_directories(mcprofiles
    PUBLIC "deps/include"
)

target_include_directories(mcprofiles PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(mcprofiles stdc++fs z)

