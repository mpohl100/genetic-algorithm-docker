set( LIB_SOURCES Lib.cpp)

add_library( lib SHARED ${LIB_SOURCES} )
target_link_libraries( lib CONAN_PKG::range-v3)
target_include_directories(lib PUBLIC {CMAKE_SOURCE_DIR}/src)