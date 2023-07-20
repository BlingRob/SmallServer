from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("boost/1.82.0")

#    def build_requirements(self):
#        self.tool_requires("cmake/3.25.1")        

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()