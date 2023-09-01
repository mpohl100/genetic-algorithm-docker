link_directories(${CMAKE_SOURCE_DIR}/src)

set( EXCEUTABLE_NAME main)

add_executable(${EXCEUTABLE_NAME} main.cpp)
target_link_libraries(${EXCEUTABLE_NAME} lib CONAN_PKG::clara)
target_include_directories(${EXCEUTABLE_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(partial partial.cpp)
target_link_libraries(partial lib CONAN_PKG::clara)
target_include_directories(partial PUBLIC ${CMAKE_SOURCE_DIR}/src)