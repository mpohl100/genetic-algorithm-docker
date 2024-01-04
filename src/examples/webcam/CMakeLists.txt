include_directories( ${CMAKE_SOURCE_DIR}/src )

set( LIB_SOURCES webcam.cpp)

add_library(webcam SHARED ${LIB_SOURCES} )
target_include_directories(webcam PUBLIC {CMAKE_SOURCE_DIR}/src)