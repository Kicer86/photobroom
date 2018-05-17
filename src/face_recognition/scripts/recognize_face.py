
import glob, os, face_recognition
import pickle

def recognize_face(face_file, known_faces_dir):

    os.chdir(known_faces_dir)

    names = []
    encoded_faces = []

    for file in glob.glob("*.jpg"):

        # try to open and read cached data
        # if there is no *.enc file, generate it
        enc_file_name = file[0:-4] + ".enc"
        try:
            enc_file = open(enc_file_name, "rb")
            encoded_raw = enc_file.read()
            enc_file.close()

            if len(encoded_raw) == 0:      # file is empty? Treat it as not existing
                raise FileNotFoundError

            if encoded_raw == b"invalid":  # cache could not be generated, skip this file
                continue

            encoded = pickle.loads(encoded_raw)

        except FileNotFoundError:          # no cache file, generate it
            print("Generating cache for file " + file)
            image = face_recognition.load_image_file(file)
            encodings = face_recognition.face_encodings(image)

            enc_file = open(enc_file_name, "ab")
            if len(encodings) > 0:
                encoded = face_recognition.face_encodings(image)[0]
                encoded_raw = pickle.dumps(encoded, protocol=0)
                enc_file.write(encoded_raw)
                enc_file.close()
            else:
                print("Could not find face")
                enc_file.write(b"invalid")
                enc_file.close()
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
