
import importlib


# verify if all required modules are available
def detect_required_modules():
    modules = ["dlib", "face_recognition", "face_recognition_models"]
    missing = []

    for module in modules:
        mod_spec = importlib.util.find_spec(module)
        found = mod_spec is not None

        if not found:
            missing.append(module)

    return missing
