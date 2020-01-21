from conans import ConanFile

class PhotoBroomConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "Exiv2/0.27@piponazo/stable", "OpenLibrary/2.3@kicer/stable"
    generators = "cmake"
    default_options = {"Exiv2:xmp": False}

    def configure(self):
        if self.settings.os == "Linux":
            self.requires.add("dlib/19.19@kicer/stable")
