cmake_minimum_required(VERSION 3.20)
project(AetherHook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_library(AetherHook STATIC ${SRC_FILES})

target_include_directories(AetherHook PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
)

file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/examples ${CMAKE_CURRENT_SOURCE_DIR}/examples/*)

foreach(subdir ${SUBDIRS})
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/${subdir})
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples/${subdir})
    endif()
endforeach()