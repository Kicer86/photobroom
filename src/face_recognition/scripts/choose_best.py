
import glob
import os
import face_recognition
import pickle
import operator

import _utils

def choose_best(faces, tmp_dir):

    # we need more than 2 faces to do any serious job
    if len(faces) < 2:
        return faces[0];

    encoded_faces = {}

    for file in faces:

        filename = os.path.basename(file)
        enc_file_name = tmp_dir + "/" + filename + ".enc"

        try:
            encoded = _utils.get_encoded(file, enc_file_name)

        except _utils.FaceNotFoundError:
            continue

        encoded_faces[file] = encoded

    results = {}

    # check each one with each one
    for file, face_data in encoded_faces:

        total_distance = 0;
        count = 0

        for file2, face_data2 in encoded_faces:
            if file == file2:
                continue

            results = face_recognition.face_distance(face_data, face_data2)
            total_distance += results[0]
            count += 1

        avg_distance = total_distance / count
        results['file'] = avg_distance

    # find photo with best avg dstance
    print(results)

    best = max(results.items(), key = operator.itemgetter(1))

    return best[0]
