# include(../common.cmake)

file(GLOB_RECURSE DOOM_EXAMPLE_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.hpp"
)

file(GLOB_RECURSE DOOM_EXAMPLE_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)


list(FILTER DOOM_EXAMPLE_SOURCES EXCLUDE REGEX "/(out|build|cmake-build-|CMakeFiles|doom_proxy)/")
list(FILTER DOOM_EXAMPLE_HEADERS EXCLUDE REGEX "/(out|build|cmake-build-|CMakeFiles|doom_proxy)/")
message(STATUS "DOOM_EXAMPLE_SOURCES: ${DOOM_EXAMPLE_SOURCES}")
# add_library(libdoom OBJECT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libdoom/doom.c" )

add_library(libdoom OBJECT "${CMAKE_CURRENT_SOURCE_DIR}/src/doom_proxy.c")
set_target_properties(libdoom PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(libdoom PUBLIC "APP_WINDOWS" "__BYTEBOOL__=1" "ALLOW_MOUSE=0")
target_include_directories(libdoom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libdoom/linuxdoom-1.10)
target_include_directories(libdoom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libdoom/libs_win32)
target_compile_options(libdoom PUBLIC "/Zc:gotoScope-" "/Zc:strictStrings-" "/wd4244" "/wd4267" "/wd4838" "/wd4430" "/wd4996" "/wd4311" "/wd4113" "/showIncludes")

add_library(doom-example STATIC ${DOOM_EXAMPLE_SOURCES} ${DOOM_EXAMPLE_HEADERS} $<TARGET_OBJECTS:libdoom>)
target_compile_definitions(doom-example PRIVATE "APP_WINDOWS" "__BYTEBOOL__=1")
target_compile_options(doom-example PUBLIC "/std:c++20" "/Zc:gotoScope-" "/showIncludes")
target_include_directories(doom-example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
target_include_directories(doom-example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libdoom/libs_win32)
target_include_directories(doom-example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libdoom/linuxdoom-1.10)
target_link_libraries(doom-example PRIVATE libtoonboom_static)
set_target_properties(doom-example PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
  )
