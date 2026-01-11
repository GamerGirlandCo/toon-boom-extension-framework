include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)


### --- sources --- ###
file(GLOB_RECURSE INJECTOR_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE INJECTOR_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

list(FILTER INJECTOR_SOURCES EXCLUDE REGEX "/(out|build|cmake-build-|CMakeFiles)/")
list(REMOVE_ITEM INJECTOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(libtoonboom_injector STATIC ${INJECTOR_SOURCES} ${INJECTOR_HEADERS})
target_compile_options(libtoonboom_injector PRIVATE "/EHsc")
target_include_directories(libtoonboom_injector PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")


add_executable(toon_boom_injector "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(toon_boom_injector PRIVATE libtoonboom_injector)
target_link_libraries(toon_boom_injector PRIVATE argparse)
target_compile_options(toon_boom_injector PRIVATE "/EHsc")


