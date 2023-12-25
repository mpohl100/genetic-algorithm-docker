include_directories(${CMAKE_SOURCE_DIR}/src)
link_directories(${CMAKE_SOURCE_DIR}/src)

find_package(clara REQUIRED)
find_package(OpenCV REQUIRED)

set( EXCEUTABLE_NAME main)

add_executable(${EXCEUTABLE_NAME} main.cpp)
target_link_libraries(${EXCEUTABLE_NAME} math clara::clara)
target_include_directories(${EXCEUTABLE_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(partial partial.cpp)
target_link_libraries(partial math clara::clara)
target_include_directories(partial PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(bubble bubbles.cpp)
target_link_libraries(bubble bubbles clara::clara)
target_include_directories(bubble PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(establishing_shot establishing_shot.cpp)
target_link_libraries(establishing_shot bubbles clara::clara)
target_include_directories(establishing_shot PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(webcam webcam.cpp)
target_include_directories(webcam PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(webcam clara::clara detection bubbles ${OpenCV_LIBS})
target_include_directories(webcam PUBLIC ${CMAKE_SOURCE_DIR}/src)