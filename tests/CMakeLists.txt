add_executable(tests tests.cpp unittest.cpp)
target_link_libraries(tests lib CONAN_PKG::catch2)
target_include_directories(tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test COMMAND $<TARGET_FILE:tests>)