file(GLOB_RECURSE SIMPLE_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE SIMPLE_HEADERS CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.hpp"
)

list(FILTER SIMPLE_SOURCES EXCLUDE REGEX "/(out|build|cmake-build-|CMakeFiles)/")
list(FILTER SIMPLE_HEADERS EXCLUDE REGEX "/(out|build|cmake-build-|CMakeFiles)/")

add_library(simple-example STATIC ${SIMPLE_SOURCES} ${SIMPLE_HEADERS})
set_target_properties(simple-example PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
  )
target_include_directories(simple-example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
target_link_libraries(simple-example PRIVATE libtoonboom_static)