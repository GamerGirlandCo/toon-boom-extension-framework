file(GLOB_RECURSE FRAMEWORK_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE FRAMEWORK_HOOK_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/hook/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/hook/*.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/hook/*.cxx"
	"${CMAKE_CURRENT_SOURCE_DIR}/hook/*.cpp"
)
file(GLOB_RECURSE FRAMEWORK_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)
list(FILTER FRAMEWORK_SOURCES EXCLUDE REGEX "/(out|build|cmake-build-|CMakeFiles)/")

file(COPY "${QT5_ROOT_DIR}/include/QtScript" DESTINATION ${CMAKE_BINARY_DIR}/include)
find_package(minhook CONFIG REQUIRED)
function(link_libs_and_set_properties target_name)
	target_compile_features(${target_name} PUBLIC cxx_std_20)
	target_include_directories(${target_name} PUBLIC "${CMAKE_BINARY_DIR}/include")
	target_link_libraries(${target_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/QtScript.lib")
	target_link_libraries(${target_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ToonBoomActionManager.lib")
	target_link_libraries(${target_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ToonBoomLayout.lib")
	target_link_libraries(${target_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ToonBoomPluginManager.lib")

	target_include_directories(${target_name} PUBLIC "${QT6_ROOT_DIR}/include")
	target_include_directories(${target_name} PUBLIC "${QT6_ROOT_DIR}/include/QtGui")

	target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/public")
	target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/internal")

	target_link_directories(${target_name} PUBLIC "${QT6_ROOT_DIR}/lib")
  target_link_libraries(${target_name} PUBLIC "${QT6_ROOT_DIR}/lib/Qt6Core.lib")
  target_link_libraries(${target_name} PUBLIC "${QT6_ROOT_DIR}/lib/Qt6Gui.lib")
  target_link_libraries(${target_name} PUBLIC "${QT6_ROOT_DIR}/lib/Qt6Widgets.lib")
  target_link_libraries(${target_name} PUBLIC "${QT6_ROOT_DIR}/lib/Qt6Core5Compat.lib")
  target_link_libraries(${target_name} PUBLIC "${QT6_ROOT_DIR}/lib/Qt6Xml.lib")
	target_compile_options(${target_name} PUBLIC "/EHsc")
	target_link_libraries(${target_name} PUBLIC minhook::minhook)
  set_target_properties(${target_name} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
  )
endfunction()

add_library(libtoonboom_objs OBJECT ${FRAMEWORK_SOURCES} ${FRAMEWORK_HEADERS} ${FRAMEWORK_HOOK_SOURCES})
set_property(TARGET libtoonboom_objs PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(libtoonboom_static STATIC $<TARGET_OBJECTS:libtoonboom_objs>)
add_library(libtoonboom SHARED $<TARGET_OBJECTS:libtoonboom_objs>)
link_libs_and_set_properties(libtoonboom_objs)
link_libs_and_set_properties(libtoonboom_static)
link_libs_and_set_properties(libtoonboom)