from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout

class ParallelConan(ConanFile):
    name = "ParallelProject"
    version = "1.0.0"
    license = "Apache License v2.0"
    author = "Michael Pohl"
    description = "A default project for a C++ application which has a library for parallelization"
    topics = ()
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "catch2/3.1.0",
        "clara/1.1.5",
        "range-v3/0.10.0",
    ]
    options = {"coverage": [True, False]}
    default_options = {"coverage": False}

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
        coverage = getattr(self.options, "coverage", False)

        if coverage:
            self.output.info("Building with coverage flags...")
            self._build_with_coverage()
        else:
            self.output.info("Building without coverage flags...")
            self._build_without_coverage()

    def _build_with_coverage(self):
        cmake = CMake(self)
        vars = {
            "ENABLE_COVERAGE": "true",
        }
        cmake.configure(vars)
        cmake.build()

    def _build_without_coverage(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
