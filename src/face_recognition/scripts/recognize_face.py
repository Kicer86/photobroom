
import glob, os, face_recognition
import pickle

import _utils

def recognize_face(face_file, known_faces_dir):

    os.chdir(known_faces_dir)

    names = []
    encoded_faces = []

    for file in glob.glob("*.jpg"):

        # try to open and read cached data
        # if there is no *.enc file, generate it
        enc_file_name = file[0:-4] + ".enc"
        try:
            encoded = _utils.get_encoded(file, enc_file_name)

        except _utils.FaceNotFoundError:
            continue

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
