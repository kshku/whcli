cmake_minimum_required(VERSION 3.15...3.31)

project(whcli VERSION 1.0.0 DESCRIPTION "Program to download the wallpapers from wallhaven")

add_executable(whcli)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(whcli PRIVATE WH_DEBUG)
endif()

add_subdirectory("external/wallhavenapi")

target_link_libraries(whcli PRIVATE whapi)

include(GNUInstallDirs)

add_subdirectory(src)
