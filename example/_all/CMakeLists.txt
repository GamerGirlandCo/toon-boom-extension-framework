file(GLOB_RECURSE EXAMPLE_LIB_SRC CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
file(GLOB_RECURSE EXAMPLE_LIB_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library(all-examples SHARED ${EXAMPLE_LIB_SRC} ${EXAMPLE_LIB_HEADERS})
target_compile_options(all-examples PUBLIC "/std:c++20" "/Zc:gotoScope-")
target_include_directories(all-examples PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(all-examples PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
  )
target_link_libraries(all-examples PUBLIC libtoonboom_static)
target_link_libraries(all-examples PRIVATE simple-example)