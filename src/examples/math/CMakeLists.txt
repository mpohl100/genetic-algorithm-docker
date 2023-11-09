include_directories( ${CMAKE_SOURCE_DIR}/src )

set( LIB_SOURCES Math.cpp)

add_library( math SHARED ${LIB_SOURCES} )
target_include_directories(math PUBLIC {CMAKE_SOURCE_DIR}/src)