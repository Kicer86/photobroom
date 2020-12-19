
from conans import ConanFile

class PhotoBroomConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "OpenLibrary/2.3@kicer/stable",
        "dlib/19.21"
    ]

    generators = "cmake"
    default_options = {}

    def configure(self):
        if self.settings.os == "Linux":
            self.options["dlib"].shared = True

    def requirements(self):
        if self.settings.os == "Windows":
            self.requires.add("zlib/1.2.11")                    # zlib is added here to avoid conflict introduced by exiv2 and dlib
            self.requires.add("Exiv2/0.27@piponazo/stable")
