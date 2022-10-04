from conans import ConanFile, CMake
from conans.model.version import Version

class CoapTeConan(ConanFile):
    name = "coap-te"
    version = "0.1"
    license = "MIT"
    author = "Rafael Cunha <rnascunha@gmail.com>"
    url = "https://github.com/rnascunha/coap-te"
    description = "CoAP RFC7252 C++17 implementation"
    topics = ("CoAP", "RFC7252", "network")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake_find_package"
    requires = "tree_trunks/[>=0.1]@base/stable"
    exports = "LICENSE", "README.md", "URL.txt"
    exports_sources = "*.hpp", "*.cpp", "CMakeLists.txt", "files.cmake", "Kconfig"

    def build_requirements(self):
        if CMake.get_version() < Version("3.10"):
            self.tool_requires("cmake/[>= 3.10]")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="src")
        self.copy("*.hpp", dst="include", src="src")
        self.copy("*.cpp", dst="src", src="src")
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.lib", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["coap-te"]

