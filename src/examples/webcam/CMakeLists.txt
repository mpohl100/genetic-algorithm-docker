include_directories( ${CMAKE_SOURCE_DIR}/src )

find_package( OpenCV REQUIRED )
find_package( Taskflow REQUIRED )

set( LIB_SOURCES webcam.cpp)

add_library(webcam SHARED ${LIB_SOURCES} )
target_link_libraries(webcam ${OpenCV_LIBS} bubbles Taskflow::Taskflow)
target_include_directories(webcam PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(webcam PUBLIC {CMAKE_SOURCE_DIR}/src)