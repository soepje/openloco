cmake_minimum_required(VERSION 4.0)
project(openloco)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL3)

include(SourceList)

add_library(openloco_lib OBJECT)

target_link_libraries(openloco_lib PRIVATE SDL3::SDL3)

add_executable(openloco)

target_link_libraries(openloco PRIVATE openloco_lib)

target_compile_definitions(openloco_lib PRIVATE SDL_MAIN_USE_CALLBACKS=1)

add_subdirectory(src)
