
import importlib


# verify if all required modules are available
def detect_required_modules():
    modules = ["dlib", "face_recognition", "face_recognition_models"]
    missing = []

    for module in modules:
        message = "Checking for " + module + " module - ";
        mod_spec = importlib.util.find_spec(module)
        found = mod_spec is not None

        if found:
            message += "found"
        else:
            message += "not found"
            missing.append(module)

        print(message)

    return missing
