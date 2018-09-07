
import glob
import os
import face_recognition
import pickle
import operator

import _utils

def choose_best(faces, tmp_dir):

    # we need at least 3 faces to do any serious job
    if len(faces) < 3:
        return faces[0];

    encoded_faces = {}

    # encode all faces for speedup
    for file in faces:

        filename = os.path.basename(file)
        enc_file_name = tmp_dir + "/" + filename + ".enc"

        try:
            encoded = _utils.get_encoded(file, enc_file_name)

        except _utils.FaceNotFoundError:
            continue

        encoded_faces[file] = encoded

    fresults = {}

    # check each one with each one
    for file, face_data in encoded_faces.items():

        total_distance = 0;
        count = 0

        for file2, face_data2 in encoded_faces.items():
            if file == file2:
                continue

            results = face_recognition.face_distance( [face_data], face_data2)
            total_distance += results[0]
            count += 1

        if count > 0:
            avg_distance = total_distance / count
            fresults[file] = avg_distance

    # find photo with best avg distance to other photos
    print(fresults)

    if len(fresults) > 0:
        best = max(fresults.items(), key = operator.itemgetter(1))

        return best[0]
    else:
        return ""
