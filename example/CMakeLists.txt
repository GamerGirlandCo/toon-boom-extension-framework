cmake_minimum_required(VERSION 3.20)
project(toon-boom-extension-framework-examples
  VERSION 0.1.0
  DESCRIPTION "Toon Boom extension framework examples"
  LANGUAGES CXX C
)
include(FetchContent)
if(NOT DEFINED IN_SUPERPROJECT)
  FetchContent_Declare(
    toon-boom-extension-framework
    GIT_REPOSITORY https://github.com/GamerGirlandCo/toon-boom-extension-framework.git
    GIT_TAG master
    FIND_PACKAGE_ARGS NAMES toon-boom-extension-framework
  )
else()
	FetchContent_Declare(
		toon-boom-extension-framework
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..
		FIND_PACKAGE_ARGS NAMES toon-boom-extension-framework
	)
endif()
FetchContent_MakeAvailable(toon-boom-extension-framework)
  find_package(toon-boom-extension-framework CONFIG REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "/Zc:__cplusplus")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Core5Compat Xml QUIET)

add_subdirectory(simple)
add_subdirectory(ultimate-artblock)
add_subdirectory(_all)