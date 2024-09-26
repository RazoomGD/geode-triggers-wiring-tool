cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(triggersWiringTool VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/ButtonListeners.cpp
    src/EditorUI.cpp
    src/EditorUtils.cpp
    src/Inits.cpp
    
    src/previewMode/ButtonListeners.cpp
    src/previewMode/Inits.cpp
    src/previewMode/MainLogic.cpp
    src/previewMode/PreviewLogic.cpp

    src/editMode/ButtonListeners.cpp
    src/editMode/EditLogic.cpp
    src/editMode/Inits.cpp
    src/editMode/Interface.cpp
    src/editMode/MainLogic.cpp
    src/editMode/Menus.cpp
    src/editMode/TouchLogic.cpp
    src/editMode/Utils.cpp
    # Add any extra C++ source files here
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
