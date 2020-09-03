from conans import ConanFile

class PhotoBroomConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "OpenLibrary/2.3@kicer/stable"
    generators = "cmake"
    default_options = {}

    def configure(self):
        if self.settings.os == "Windows":
            self.requires.add("Exiv2/0.27@piponazo/stable")
            self.default_options = {"Exiv2:xmp": False}
