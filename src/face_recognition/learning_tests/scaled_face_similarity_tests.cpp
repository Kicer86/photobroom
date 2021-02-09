
#include <algorithm>
#include <gtest/gtest.h>
#include <QImage>
#include <QString>

#include <unit_tests_utils/empty_logger.hpp>
#include "face_recognition/dlib_wrapper/dlib_face_recognition_api.hpp"
#include "utils.hpp"

namespace
{
    QImage extractFace(const QImage& photo)
    {
        EmptyLogger logger;

        QVector faces = dlib_api::FaceLocator(&logger).face_locations_cnn(photo, 0);
        assert(faces.size() == 1);

        const QRect faceRect = faces.front();
        const QImage faceImg = photo.copy(faceRect);

        return faceImg;
    }


    void faceDetectionTest(const QImage& img, QVector<QRect>(dlib_api::FaceLocator::*face_locator)(const QImage &, int))
    {
        EmptyLogger logger;

        dlib_api::FaceLocator faceLocator(&logger);

        // original image size
        {
            QVector faces = (faceLocator.*face_locator)(img, 0);
            EXPECT_EQ(faces.size(), 1);
        }

        // double size
        if (face_locator == &dlib_api::FaceLocator::face_locations_hog)         // cnn may fail due to memory allocation error
        {
            QVector faces = faceLocator.face_locations_hog(img, 1);
            EXPECT_EQ(faces.size(), 1);
        }

        // downsize by 2
        const QImage small = utils::downsize(img, 2);
        QVector faces = (faceLocator.*face_locator)(small, 0);
        EXPECT_EQ(faces.size(), 1);

        // it may be imposible to find face on smaller images
        const QImage smaller = utils::downsize(img, 4);
        faces = (faceLocator.*face_locator)(smaller, 0);
        EXPECT_EQ(faces.size(), 0);
    }
}

class FaceScalingTest: public testing::Test
{
public:
    FaceScalingTest()
        : img1(utils::photoSetPath() + "/George_W_Bush/George_W_Bush_0015.jpg")
        , img2(utils::photoSetPath() + "/Geoff_Hoon/Geoff_Hoon_0003.jpg")
    {
    }

    const QImage img1;
    const QImage img2;
};


TEST_F(FaceScalingTest, faceDetectionForCnn)
{
    faceDetectionTest(img1, &dlib_api::FaceLocator::face_locations_cnn);
    faceDetectionTest(img2, &dlib_api::FaceLocator::face_locations_cnn);
}


TEST_F(FaceScalingTest, faceDetectionForHog)
{
    faceDetectionTest(img1, &dlib_api::FaceLocator::face_locations_hog);
    faceDetectionTest(img2, &dlib_api::FaceLocator::face_locations_hog);
}


TEST_F(FaceScalingTest, scaledFaceDistance)
{
    dlib_api::FaceEncoder faceEncoder(&logger);

    const QImage img(img1);
    const QImage face = extractFace(img);
    const auto face_encodings = faceEncoder.face_encodings(face);

    std::vector<dlib_api::FaceEncodings> faces_encodings = { face_encodings };

    // upsize by factor of powers of 2
    for(int scale = 2; scale <= 8; scale++)
    {
        const QImage scaled_image = utils::upsize(img, scale);

        const QImage scaled_face = extractFace(scaled_image);
        const auto scaled_face_encodings = faceEncoder.face_encodings(scaled_face);
        faces_encodings.push_back(scaled_face_encodings);
    }

    // compare distance between each image
    const std::size_t s = faces_encodings.size();

    for(std::size_t i = 0; i < s; i++)
        for(std::size_t j = i + 1; j < s; j++)
        {
            const auto& lhs = faces_encodings[i];
            const auto& rhs = faces_encodings[j];

            const std::vector distances = dlib_api::face_distance( {lhs}, rhs );
            const double distance = distances.front();

            EXPECT_LT(distance, 0.3);
        }
}
