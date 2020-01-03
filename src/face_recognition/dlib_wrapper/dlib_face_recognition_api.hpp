
#ifndef DLIB_FACE_RECOGNITION_API_HPP_INCLUDED
#define DLIB_FACE_RECOGNITION_API_HPP_INCLUDED


#include <vector>
#include <QImage>
#include <QRect>

// based on:
// https://github.com/ageitgey/face_recognition/blob/master/face_recognition/api.py

namespace dlib_api
{
    std::vector<QRect> face_locations(const QImage& img, int number_of_times_to_upsample=1, const std::string& model="hog");
}

#endif // DLIB_FACE_RECOGNITION_API_HPP_INCLUDED
