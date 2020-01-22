
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

const QString face1Path = QString(PATH(FACES_DIR)) + "/face1.jpg";
const QString face2Path = QString(PATH(FACES_DIR)) + "/face2.jpg";

QImage downsize(const QImage& source, int factor)
{
    const QSize size = source.size();
    const qreal scaleFactor = std::sqrt(factor);
    const QSize scaledSize = size / scaleFactor;

    return source.scaled(scaledSize);
}


TEST(FaceScalingTest, faceDetectionForCnn)
{
    const QImage img(face1Path);

    ASSERT_EQ(img.size(), QSize(1024, 1024));

    // original image size
    {
        QVector faces = dlib_api::face_locations(img, 0, dlib_api::cnn);
        EXPECT_EQ(faces.size(), 1);
    }

    // double size  ( disabled as out of memory exception breaks next detection )
    //{
    //    QVector faces = dlib_api::face_locations(img, 1, dlib_api::cnn);
    //    EXPECT_EQ(faces.size(), 0);     // fail expected due to memory consumption
    //}

    // downsize by factor of powers of 2 - minimal image size is 128x128, detected face size is left: 12, right: 90, top: 28, bottom: 106
    for(int scale = 2; scale <= 64; scale *= 2)
    {
        const QImage small = downsize(img, scale);
        QVector faces = dlib_api::face_locations(small, 0, dlib_api::cnn);
        EXPECT_EQ(faces.size(), 1);
    }

    // downsize by factor of 128  - image size 91x91 - too small to detect face
    {
        const QImage small = downsize(img, 128);
        QVector faces = dlib_api::face_locations(small, 0, dlib_api::cnn);
        EXPECT_EQ(faces.size(), 0);
    }
}
