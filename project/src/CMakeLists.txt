set( LIB_SOURCES Math.cpp)

add_library( lib SHARED ${LIB_SOURCES} )
target_include_directories(lib PUBLIC {CMAKE_SOURCE_DIR}/src)