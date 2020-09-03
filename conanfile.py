from conans import ConanFile

class PhotoBroomConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "OpenLibrary/2.3@kicer/stable"
    generators = "cmake"
