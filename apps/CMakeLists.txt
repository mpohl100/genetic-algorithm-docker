include_directories(${CMAKE_SOURCE_DIR}/src)
link_directories(${CMAKE_SOURCE_DIR}/src)

find_package(clara REQUIRED)

set( EXCEUTABLE_NAME main)

add_executable(${EXCEUTABLE_NAME} main.cpp)
target_link_libraries(${EXCEUTABLE_NAME} math clara::clara)
target_include_directories(${EXCEUTABLE_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(partial partial.cpp)
target_link_libraries(partial math clara::clara)
target_include_directories(partial PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(adjust adjust.cpp)
target_link_libraries(adjust math clara::clara)
target_include_directories(adjust PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(bubble bubbles.cpp)
target_link_libraries(bubble bubbles math2d clara::clara)
target_include_directories(bubble PUBLIC ${CMAKE_SOURCE_DIR}/src)