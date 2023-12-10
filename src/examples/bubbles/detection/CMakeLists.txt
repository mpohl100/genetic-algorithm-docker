
find_package(OpenCV REQUIRED)

set( DETECTION_SOURCES Detection.cpp)
add_library(detection SHARED ${DETECTION_SOURCES} )
target_include_directories(detection PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(detection ${OpenCV_LIBS})
target_include_directories(detection PUBLIC {CMAKE_SOURCE_DIR}/src)