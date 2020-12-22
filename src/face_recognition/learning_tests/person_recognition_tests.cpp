
#include <gtest/gtest.h>
#include <QDirIterator>

#include <unit_tests_utils/empty_logger.hpp>

#include "utils.hpp"
#include "face_recognition/dlib_wrapper/dlib_face_recognition_api.hpp"

namespace
{
    dlib_api::FaceEncodings fetchFaceEncodings(dlib_api::FaceLocator& locator, dlib_api::FaceEncoder& encoder, const QString& path)
    {
        dlib_api::FaceEncodings result;
        const QImage photo(path);

        const auto location = locator.face_locations(photo);
        if (location.size(), 1)
        {
            const QImage face = photo.copy(location.front());
            const auto encodings = encoder.face_encodings(face);

            result = encodings;
        }

        return result;
    }

}


TEST(PersonRecognitionTest, GeorgeWBushRecognition)
{
    EmptyLogger logger;
    dlib_api::FaceLocator locator(&logger);
    dlib_api::FaceEncoder encoder;
    QDirIterator di(utils::photoSetPath() + "/George_W_Bush", QDir::Files);

    const QString referencePhotoPath = utils::photoSetPath() + "/George_W_Bush/George_W_Bush_0006.jpg";
    const auto referenceFaceEncoding = fetchFaceEncodings(locator, encoder, referencePhotoPath);
    ASSERT_FALSE(referenceFaceEncoding.empty());

    std::vector<dlib_api::FaceEncodings> testedFacesEncodings;

    while(di.hasNext())
    {
        const QString file(di.next());

        const auto testedFaceEncoding = fetchFaceEncodings(locator, encoder, file);
        if (testedFaceEncoding.empty() == false)
            testedFacesEncodings.push_back(testedFaceEncoding);
    }

    const auto distances = dlib_api::face_distance(testedFacesEncodings, referenceFaceEncoding);

    const auto matching = std::count_if(distances.cbegin(), distances.cend(), [](const auto& distance)
    {
        return distance <= 0.6;
    });

    // There are 530 photos of George W Bush. With current implementation and settings only 510 are matching
    EXPECT_EQ(matching, 510);
}
