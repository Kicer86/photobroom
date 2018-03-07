
import glob, os, face_recognition

def recognize_face(face_file, known_faces_dir):

    os.chdir(known_faces_dir)

    names = []
    encoded_faces = []

    for file in glob.glob("*.jpg"):

        image = face_recognition.load_image_file(file)
        encoded = face_recognition.face_encodings(image)[0]

        names.append(file)
        encoded_faces.append(encoded)

    unknown_face_file = face_recognition.load_image_file(face_file)

    try:
        unknown_face_encoded = face_recognition.face_encodings(unknown_face_file)[0]
    except IndexError:
        return str()

    results = face_recognition.compare_faces(encoded_faces, unknown_face_encoded)

    for i in range(0, len(results)):
        if results[i]:
            return names[i]

    return str()
