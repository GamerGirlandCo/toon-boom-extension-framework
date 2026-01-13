cmake_minimum_required(VERSION 3.20)
project(toon-boom-extension-framework
  VERSION 0.1.0
  DESCRIPTION "Toon Boom extension framework"
  LANGUAGES CXX
)

if(NOT DEFINED VCPKG_ROOT)
  message(FATAL_ERROR "VCPKG_ROOT is not defined. Please set the VCPKG_ROOT variable to the root directory of the vcpkg installation.")
endif()

if(NOT DEFINED QT5_ROOT_DIR)
  message(FATAL_ERROR "QT5_ROOT_DIR is not defined. Please set the QT5_ROOT_DIR variable to the root directory of the Qt 5.15 installation.")
endif()

if(NOT DEFINED QT6_ROOT_DIR)
  message(FATAL_ERROR "QT6_ROOT_DIR is not defined. Please set the QT6_ROOT_DIR variable to the root directory of the Qt 6 installation.")
endif()

include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
list(APPEND CMAKE_PREFIX_PATH ${QT6_ROOT_DIR})
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /Zc:__cplusplus")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Core5Compat Xml QUIET)
add_subdirectory(framework)
add_subdirectory(injector)