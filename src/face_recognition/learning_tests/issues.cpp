
#include <gtest/gtest.h>
#include <QImage>
#include <dlib/cuda/cuda_errors.h>

#include "face_recognition/dlib_wrapper/dlib_face_recognition_api.hpp"
#include "utils.hpp"

TEST(DlibIssuesTest, outOfMemory)
{
    const QImage img1(utils::photoPath(1));

    dlib_api::FaceLocator faceLocator;
    QVector facesCnn = faceLocator.face_locations(img1, 0, dlib_api::cnn);
    ASSERT_EQ(facesCnn.size(), 1);

    ASSERT_THROW({
        for(int i = 1; i < 128; i++)
            faceLocator.face_locations_cnn(img1, i);   // locate faces with 'i' getting higher and higher until cuda throws out of memory exception
    }, dlib::cuda_error);

    EXPECT_THROW({
        faceLocator.face_locations_cnn(img1, 0);       // dlib 19.19 throws even for valid images if it threw previously
    },
    dlib::cuda_error);
}
