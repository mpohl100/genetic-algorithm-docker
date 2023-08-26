add_library( evol SHARED Rng.cpp )
target_include_directories(evol PUBLIC {CMAKE_SOURCE_DIR}/src/evol)