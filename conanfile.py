from conans import ConanFile, CMake


class OsMatcherConan(ConanFile):
    name = "os-matcher"
    version = "0.5"

    # Optional metadata
    license = "Apache-2.0"
    author = "Ambrosys GmbH - info@ambrosys.de"
    url = "https://github.com/Ambrosys/os-matcher"
    homepage = url
    description = "OS-Matcher is an open-source C++ framework for routing applications."
    topics = ("street matching")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "testing": [True, False]}
    default_options = {"shared": False, "fPIC": True, "testing": True}
    generators = "cmake", "cmake_find_package"
    exports = "LICENSE"
    exports_sources = "CMakeLists.txt", "include/*", "CMakeModules/*", "src/*"
    no_copy_source = True
    requires = [
        ("amb-log/0.5.0@amb/stable"),
        ("amb-pipeline/0.4.0@amb/stable"),
        ("cli-app/0.4.0@amb/stable"),
        ("nlohmann_json/3.9.1"),
        ("boost/1.76.0"),
        ("libpqxx/7.5.0"),
        ("lemon/1.3.2@amb/stable")
    ]

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.definitions["BUILD_UNITTESTS"] = self.options.testing
        cmake.definitions["CONAN_DISABLE_CHECK_COMPILER"] = True
        return cmake

    def requirements(self):
        if self.options.testing:
            self.requires("catch2/2.13.6")

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="*.h", dst="include/", src="src/", keep_path=True)
        self.copy(pattern="LICENSE")
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["os-matcher"]
