import os

from conans import ConanFile, CMake, tools

class OsMatcherTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "src/*"
    generators = "cmake_find_package", "cmake"
    requires = [("os-matcher/0.1@amb/stable"), ("amb-log/0.5.0@amb/stable"), ("cli-app/0.4.0@amb/stable")]

    def validate(self):
        tools.check_min_cppstd(self, "17")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy(pattern="*.dll", src="bin", dst="bin")
        self.copy(pattern="*.dylib*", src="lib", dst="bin")
        self.copy(pattern='*.so*', src='lib', dst='bin')

    def test(self):
        if not tools.cross_building(self):
            os.chdir("bin")
            self.run(".%sexample" % os.sep)