
#ifndef DLIB_FACE_RECOGNITION_API_HPP_INCLUDED
#define DLIB_FACE_RECOGNITION_API_HPP_INCLUDED


#include <QImage>
#include <QRect>
#include <QVector>

// based on:
// https://github.com/ageitgey/face_recognition/blob/master/face_recognition/api.py

namespace dlib_api
{
    enum Model
    {
        hog,
        cnn,
    };

    enum EncodingsModel
    {
        small,
        large,
    };

    // https://github.com/ageitgey/face_recognition/blob/5fe85a1a8cbd1b994b505464b555d12cd25eee5f/face_recognition/api.py#L108
    QVector<QRect> face_locations(const QImage &, int number_of_times_to_upsample = 1, Model = hog);

    // https://github.com/ageitgey/face_recognition/blob/5fe85a1a8cbd1b994b505464b555d12cd25eee5f/face_recognition/api.py#L203
    QByteArray face_encodings(const QImage& face, int num_jitters = 1, EncodingsModel = large);
}

#endif // DLIB_FACE_RECOGNITION_API_HPP_INCLUDED
