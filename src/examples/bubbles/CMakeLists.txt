include_directories( ${CMAKE_SOURCE_DIR}/src )

add_subdirectory(detection)

set( BUBBLES_SOURCES Canvas2D.cpp bubbles_swarm.cpp establishing_frame.cpp math2d.cpp Tiles.cpp)

add_library( bubbles SHARED ${BUBBLES_SOURCES} )
target_include_directories(bubbles PUBLIC {CMAKE_SOURCE_DIR}/src)
target_link_libraries(bubbles detection)