
#include <algorithm>
#include <gtest/gtest.h>
#include <QImage>
#include <QString>

#include "face_recognition/dlib_wrapper/dlib_face_recognition_api.hpp"
#include "utils.hpp"

namespace
{
    QImage extractFace(const QImage& photo)
    {
        QVector faces = dlib_api::FaceLocator().face_locations_cnn(photo, 0);
        assert(faces.size() == 1);

        const QRect faceRect = faces.front();
        const QImage faceImg = photo.copy(faceRect);

        return faceImg;
    }


    void faceDetectionTest(const QImage& img, QVector<QRect>(dlib_api::FaceLocator::*face_locator)(const QImage &, int))
    {
        dlib_api::FaceLocator faceLocator;

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

        // downsize by factor of powers of 2 - minimal image size is 128x128, detected face size is left: 12, right: 90, top: 28, bottom: 106
        for(int scale = 2; scale <= 64; scale *= 2)
        {
            const QImage small = utils::downsize(img, scale);
            QVector faces = (faceLocator.*face_locator)(img, 0);
            EXPECT_EQ(faces.size(), 1);
        }

        // it may be imposible to find face on smaller images
    }
}


TEST(FaceScalingTest, faceDetectionForCnn)
{
    const QImage img1(utils::photoPath(1));
    const QImage img2(utils::photoPath(2));

    faceDetectionTest(img1, &dlib_api::FaceLocator::face_locations_cnn);
    faceDetectionTest(img2, &dlib_api::FaceLocator::face_locations_cnn);
}


TEST(FaceScalingTest, faceDetectionForHog)
{
    const QImage img1(utils::photoPath(1));
    const QImage img2(utils::photoPath(2));

    faceDetectionTest(img1, &dlib_api::FaceLocator::face_locations_hog);
    faceDetectionTest(img2, &dlib_api::FaceLocator::face_locations_hog);
}


TEST(FaceScalingTest, scaledFaceDistance)
{
    dlib_api::FaceEncoder faceEncoder;

    const QImage img(utils::photoPath(1));
    const QImage face = extractFace(img);
    const auto face_encodings = faceEncoder.face_encodings(face);

    std::vector<dlib_api::FaceEncodings> faces_encodings = { face_encodings };

    // downsize by factor of powers of 2
    // minimal image size is 128x128, detected face size is left: ~12, right: ~90, top: ~28, bottom: ~106
    for(int scale = 2; scale <= 64; scale *= 2)
    {
        const QImage small_image = utils::downsize(img, scale);

        const QImage small_face = extractFace(small_image);
        const auto small_face_encodings = faceEncoder.face_encodings(small_face);
        faces_encodings.push_back(small_face_encodings);
    }

    // compare distance between each image
    const std::size_t s = faces_encodings.size();
    std::vector<std::vector<double>> distances_matrix(s, std::vector<double>());

    for(std::size_t i = 0; i < s; i++)
        for(std::size_t j = i + 1; j < s; j++)
        {
            const auto& lhs = faces_encodings[i];
            const auto& rhs = faces_encodings[j];

            const std::vector distances = dlib_api::face_distance( {lhs}, rhs );
            const double distance = distances.front();

            distances_matrix[i].push_back(distance);
            distances_matrix[j].push_back(distance);
        }

    // calculate mean, min and max for each face
    std::vector<double> means;
    std::vector<double> mins;
    std::vector<double> maxs;

    for(const auto& distances: distances_matrix)
    {
        const auto [min, max] = std::minmax_element(distances.cbegin(), distances.cend());
        const double sum = std::accumulate(distances.cbegin(), distances.cend(), 0.0);
        const double mean = sum/distances.size();

        means.push_back(mean);
        mins.push_back(*min);
        maxs.push_back(*max);
    }

    // it seams that photos 512x512 (downsized by 2) ÷ 181x181 (downsized by 32) have best means
    auto mean_best = std::min_element(means.cbegin(), means.cend());
    const int pos = std::distance(means.cbegin(), mean_best);

    EXPECT_NE(pos, 0);      // 1024x1024 has high mean
    EXPECT_NE(pos, s-1);    // 128x128 also has high mean
}
