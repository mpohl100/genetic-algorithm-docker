add_executable(tests tests.cpp unittest.cpp math.cpp canvas.cpp bubbles_algo.cpp)
target_link_libraries(tests math bubbles CONAN_PKG::catch2)
target_include_directories(tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test COMMAND $<TARGET_FILE:tests>)