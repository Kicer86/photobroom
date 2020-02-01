
#ifndef DLIB_FACE_RECOGNITION_API_HPP_INCLUDED
#define DLIB_FACE_RECOGNITION_API_HPP_INCLUDED


#include <QImage>
#include <QRect>
#include <QVector>
#include <memory>
#include <optional>

#include "dlib_wrapper_export.h"

// based on:
// https://github.com/ageitgey/face_recognition/blob/master/face_recognition/api.py

namespace dlib_api
{
    enum Model
    {
        hog,            // cpu based
        cnn,            // gpu based (may return empty set in case of gpu error like memory allocation problems)
        automatic,      // uses gpu model, and if it fails, uses cpu
    };

    enum EncodingsModel
    {
        small,
        large,
    };

    typedef std::vector<double> FaceEncodings;

    // FaceLocator may be a very heavy object. Delete it as soon as it is not needed.
    class DLIB_WRAPPER_EXPORT FaceLocator
    {
        public:
            FaceLocator();
            ~FaceLocator();

            // https://github.com/ageitgey/face_recognition/blob/5fe85a1a8cbd1b994b505464b555d12cd25eee5f/face_recognition/api.py#L108
            QVector<QRect> face_locations(const QImage &, int number_of_times_to_upsample = 1, Model = cnn);

            QVector<QRect> face_locations_cnn(const QImage &, int number_of_times_to_upsample = 1);   // may throw an exception

        private:
            struct Data;
            std::unique_ptr<Data> m_data;

            std::optional<QVector<QRect>> _face_locations_cnn(const QImage &, int);
            QVector<QRect> face_locations_hog(const QImage &, int);
    };

    class DLIB_WRAPPER_EXPORT FaceEncoder
    {
        public:
            FaceEncoder();
            ~FaceEncoder();

            // https://github.com/ageitgey/face_recognition/blob/5fe85a1a8cbd1b994b505464b555d12cd25eee5f/face_recognition/api.py#L203
            std::vector<double> face_encodings(const QImage& face, int num_jitters = 1, EncodingsModel = large);

        private:
            struct Data;
            std::unique_ptr<Data> m_data;
    };

    // https://github.com/ageitgey/face_recognition/blob/5fe85a1a8cbd1b994b505464b555d12cd25eee5f/face_recognition/api.py#L217
    DLIB_WRAPPER_EXPORT std::vector<bool> compare_faces(const std::vector<FaceEncodings>& known_face_encodings, const FaceEncodings& face_encoding_to_check, double tolerance = .6);

    // https://github.com/ageitgey/face_recognition/blob/5fe85a1a8cbd1b994b505464b555d12cd25eee5f/face_recognition/api.py#L63
    DLIB_WRAPPER_EXPORT std::vector<double> face_distance(const std::vector<FaceEncodings>& face_encodings, const FaceEncodings& face_to_compare);
}

#endif // DLIB_FACE_RECOGNITION_API_HPP_INCLUDED
