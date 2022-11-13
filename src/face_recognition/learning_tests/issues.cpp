
#ifdef DLIB_HAS_CUDA_MEMORY_OVERFLOW_BUG

#include <gtest/gtest.h>
#include <QImage>
#include <dlib/cuda/cuda_errors.h>

#include <unit_tests_utils/empty_logger.hpp>
#include "face_recognition/dlib_wrapper/dlib_face_recognition_api.hpp"
#include "utils.hpp"


namespace
{
    EmptyLogger logger;
    const QImage img1(utils::photoSetPath() + "/Ronald_Reagan/Ronald_Reagan_0001.jpg");
}


TEST(DlibIssuesTest, outOfMemory)
{
    dlib_api::FaceLocator faceLocator(&logger);
    QVector facesCnn = faceLocator.face_locations_cnn(img1, 0);
    ASSERT_EQ(facesCnn.size(), 1);

    ASSERT_THROW({
        for(int i = 1; i < 128; i++)
            faceLocator.face_locations_cnn(img1, i);   // locate faces with 'i' getting higher and higher until cuda throws out of memory exception
    }, dlib::cuda_error);

    EXPECT_THROW({
        faceLocator.face_locations_cnn(img1, 0);       // dlib 19.19 throws even for valid images if it threw previously
    },
    dlib::cuda_error);

    EXPECT_NO_THROW({
        faceLocator.face_locations_cnn(img1, 0);       // but does not throw for yet another call
    });
}


TEST(DlibIssuesTest, outOfMemoryEvenWhenResourcesAreAvailable)
{
    {
        dlib_api::FaceLocator faceLocator(&logger);
        QVector facesCnn = faceLocator.face_locations_cnn(img1, 0);
        ASSERT_EQ(facesCnn.size(), 1);

        ASSERT_THROW({
            for(int i = 1; i < 128; i++)
                faceLocator.face_locations_cnn(img1, i);   // locate faces with 'i' getting higher and higher until cuda throws out of memory exception
        }, dlib::cuda_error);
    }

    {
        dlib_api::FaceLocator faceLocator(&logger);

        EXPECT_THROW({
            faceLocator.face_locations_cnn(img1, 0);       // another instance of FaceLocator so we should be fine here, but well, surprise
        },
        dlib::cuda_error);
    }
}

#endif
