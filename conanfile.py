from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout

class VideoSentinelConan(ConanFile):
    name = "DefaultProject"
    version = "1.0.0"
    license = "Apache License v2.0"
    author = "Michael Pohl"
    description = "A default project for a C++ application"
    topics = ()
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "catch2/3.1.0",
        "clara/1.1.5",
        "range-v3/0.10.0",
    ]

    def configure(self):
        print("do nothing in configure")

    def requirements(self):
        print("do nothing in requirements")

    def layout(self):
        cmake_layout(self, src_folder=".")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
