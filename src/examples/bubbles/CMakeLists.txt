include_directories( ${CMAKE_SOURCE_DIR}/src )

set( BUBBLES_SOURCES Canvas2D.cpp bubbles_swarm.cpp)

add_library( bubbles SHARED ${BUBBLES_SOURCES} )
target_include_directories(bubbles PUBLIC {CMAKE_SOURCE_DIR}/src)