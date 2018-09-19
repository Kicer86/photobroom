
import face_recognition
import pickle


class FaceNotFoundError(BaseException):
    pass


def get_encoded(image_file, enc_file_name):

    # try to open and read cached data
    # if there is no *.enc file, generate it

    try:
        enc_file = open(enc_file_name, "rb")
        encoded_raw = enc_file.read()
        enc_file.close()

        if len(encoded_raw) == 0:      # file is empty? Treat it as not existing
            raise FileNotFoundError

        if encoded_raw == b"invalid":  # cache says we could not find face
            raise FaceNotFoundError

        encoded = pickle.loads(encoded_raw)

    except FileNotFoundError:          # no cache file, generate it
        print("Generating cache for file " + image_file)
        image = face_recognition.load_image_file(image_file)
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

            raise FaceNotFoundError

    return encoded
