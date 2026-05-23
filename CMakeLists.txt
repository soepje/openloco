cmake_minimum_required(VERSION 4.0)
project(openloco)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(SDL EXCLUDE_FROM_ALL)

file(GLOB_RECURSE CPP_HEADERS src/*.hpp)
file(GLOB_RECURSE CPP_SOURCES src/*.cpp)

add_executable(openloco ${CPP_HEADERS} ${CPP_SOURCES})

target_link_libraries(openloco PRIVATE SDL3::SDL3)
target_compile_definitions(openloco PRIVATE SDL_MAIN_USE_CALLBACKS=1)

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --embed-file ${PROJECT_SOURCE_DIR}/art-res@/art-res")
endif()
