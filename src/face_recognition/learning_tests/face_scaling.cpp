
#include <cmath>
#include <gtest/gtest.h>
#include <QImage>
#include <QString>

#include "face_recognition/dlib_wrapper/dlib_face_recognition_api.hpp"

#ifndef FACES_DIR
#error FACES_DIR is not set
#endif

#define STRING(s) #s
#define PATH(path) STRING(path)

namespace
{
    const QString face1Path = QString(PATH(FACES_DIR)) + "/face1.jpg";
    const QString face2Path = QString(PATH(FACES_DIR)) + "/face2.jpg";

    QImage downsize(const QImage& source, int factor)
    {
        const QSize size = source.size();
        const qreal scaleFactor = std::sqrt(factor);
        const QSize scaledSize = size / scaleFactor;

        return source.scaled(scaledSize);
    }


    QImage extractFace(const QImage& photo)
    {
        QVector faces = dlib_api::face_locations(photo, 0, dlib_api::cnn);
        assert(faces.size() == 1);

        const QRect faceRect = faces.front();
        const QImage faceImg = photo.copy(faceRect);

        return faceImg;
    }

    void faceDetectionTest(const QImage& img, dlib_api::Model model)
    {
        // original image size
        {
            QVector faces = dlib_api::face_locations(img, 0, model);
            EXPECT_EQ(faces.size(), 1);
        }

        // double size
        if (model == dlib_api::hog)         // cnn may fail due to memory allocation error
        {
            QVector faces = dlib_api::face_locations(img, 1, model);
            EXPECT_EQ(faces.size(), 1);
        }

        // downsize by factor of powers of 2 - minimal image size is 128x128, detected face size is left: 12, right: 90, top: 28, bottom: 106
        for(int scale = 2; scale <= 64; scale *= 2)
        {
            const QImage small = downsize(img, scale);
            QVector faces = dlib_api::face_locations(small, 0, model);
            EXPECT_EQ(faces.size(), 1);
        }

        // it may be imposible to find face on smaller images
    }
}


TEST(FaceScalingTest, prerequisites)
{
    const QImage img1(face1Path);
    const QImage img2(face2Path);

    ASSERT_EQ(img1.size(), QSize(1024, 1024));
    ASSERT_EQ(img2.size(), QSize(1024, 1024));
}


TEST(FaceScalingTest, faceDetectionForCnn)
{
    const QImage img1(face1Path);
    const QImage img2(face2Path);

    faceDetectionTest(img1, dlib_api::cnn);
    faceDetectionTest(img2, dlib_api::cnn);
}


TEST(FaceScalingTest, faceDetectionForHog)
{
    const QImage img1(face1Path);
    const QImage img2(face2Path);

    faceDetectionTest(img1, dlib_api::hog);
    faceDetectionTest(img2, dlib_api::hog);
}


TEST(FaceScalingTest, scaledFaceDistance)
{
    const QImage img(face1Path);
    const QImage face = extractFace(img);
    const auto face_encodings = dlib_api::face_encodings(face);

    // downsize by factor of powers of 2 - minimal image size is 128x128, detected face size is left: 12, right: 90, top: 28, bottom: 106
    for(int scale = 2; scale <= 64; scale *= 2)
    {
        const QImage small_image = downsize(img, scale);
        const QImage small_face = extractFace(small_image);
        const auto small_face_encodings = dlib_api::face_encodings(small_face);
        const std::vector distances = dlib_api::face_distance( {small_face_encodings}, face_encodings );
        const double distance = distances.front();

        EXPECT_LT(distance, 0.25);
    }
}
