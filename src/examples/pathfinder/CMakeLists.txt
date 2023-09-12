set( PATH_SOURCES Canvas2D.cpp)

add_library( path SHARED ${PATH_SOURCES} )
target_include_directories(path PUBLIC {CMAKE_SOURCE_DIR}/src)