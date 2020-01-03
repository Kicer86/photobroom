
#ifndef DLIB_FACE_RECOGNITION_API_HPP_INCLUDED
#define DLIB_FACE_RECOGNITION_API_HPP_INCLUDED


#include <QImage>
#include <QRect>
#include <QVector>

// based on:
// https://github.com/ageitgey/face_recognition/blob/master/face_recognition/api.py

namespace dlib_api
{
    QVector<QRect> face_locations(const QImage& img, int number_of_times_to_upsample=1, const std::string& model="hog");
}

#endif // DLIB_FACE_RECOGNITION_API_HPP_INCLUDED
