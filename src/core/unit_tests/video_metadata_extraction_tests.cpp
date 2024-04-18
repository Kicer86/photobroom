
#include <gtest/gtest.h>

#include "config.hpp"
#include "media_information.hpp"
#include "exif_reader_factory.hpp"
#include "unit_tests_utils/mock_core_factory_accessor.hpp"
#include "unit_tests_utils/mock_logger_factory.hpp"
#include "unit_tests_utils/empty_logger.hpp"
#include "unit_tests_utils/printers.hpp"


#include <constants.hpp>


using testing::An;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;
using testing::ReturnRef;


struct VideoMediaInformationTestParam
{
    std::string_view path;
    QDateTime creation_time;
    QSize dimension;
    std::chrono::milliseconds duration;
};

class VideoMediaInformationTest: public testing::TestWithParam<VideoMediaInformationTestParam> {};

INSTANTIATE_TEST_SUITE_P(
    SampleVideos, VideoMediaInformationTest,
    testing::Values
    (
        VideoMediaInformationTestParam
        {
            PHOTO_BROOM_SRC_DIR("src/core/learning_tests/test-videos/Atoms - 8579.mp4"),
            {},
            QSize(1920, 1080),
            std::chrono::milliseconds(14180)
        },
        VideoMediaInformationTestParam
        {
            PHOTO_BROOM_SRC_DIR("src/core/learning_tests/test-videos/Blue_Sky_and_Clouds_Timelapse_0892__Videvo.mov"),
            QDateTime(QDate(2014, 6, 6), QTime(13, 48, 40)),
            QSize(1920, 1080),
            std::chrono::milliseconds(15600)
        },
        VideoMediaInformationTestParam
        {
            PHOTO_BROOM_SRC_DIR("src/core/learning_tests/test-videos/close-up-of-flowers-13554420.mp4"),
            QDateTime(QDate(2022, 9, 7), QTime(18, 33, 11)),
            QSize(2160, 3840),
            std::chrono::milliseconds(8020)
        },
        VideoMediaInformationTestParam
        {
            PHOTO_BROOM_SRC_DIR("src/core/learning_tests/test-videos/DSC_8073.MP4"),
            QDateTime(QDate(2024, 4, 10), QTime(19, 48, 02)),
            QSize(3840, 2160),
            std::chrono::milliseconds(3403)
        },
        VideoMediaInformationTestParam
        {
            PHOTO_BROOM_SRC_DIR("src/core/learning_tests/test-videos/fog-over-mountainside-13008647.mp4"),
            QDateTime(QDate(2022, 7, 28), QTime(9, 29, 06)),
            QSize(2160, 3240),
            std::chrono::milliseconds(7146)
        },
        VideoMediaInformationTestParam
        {
            PHOTO_BROOM_SRC_DIR("src/core/learning_tests/test-videos/orientations/VID_20240412_181512.mp4"),
            // TODO: expected date and time are: 2024.04.12 18:15:21. However I found no way to get it from file metadata.
            //       Therefore as of now, time in UTC (which is valid (with +3sec margin)) is being used
            QDateTime(QDate(2024, 4, 12), QTime(16, 15, 15)),
            QSize(1080, 1920),
            std::chrono::milliseconds(1673)
        },
        VideoMediaInformationTestParam
        {
            PHOTO_BROOM_SRC_DIR("src/core/learning_tests/test-videos/orientations/VID_20240412_181526.mp4"),
            // TODO: expected date and time are: 2024.04.12 18:15:26. However I found no way to get it from file metadata.
            //       Therefore as of now, time in UTC (which is valid (with +5sec margin)) is being used
            QDateTime(QDate(2024, 4, 12), QTime(16, 15, 31)),
            QSize(1080, 1920),
            std::chrono::milliseconds(3370)
        }
    ));


TEST_P(VideoMediaInformationTest, validateMetadata)
{
    const auto& [path, creationTime, dimension, duration] = GetParam();
    NiceMock<ICoreFactoryAccessorMock> core;
    NiceMock<ILoggerFactoryMock> loggerFactory;
    ExifReaderFactory exifFactory;

    ON_CALL(core, getLoggerFactory).WillByDefault(ReturnRef(loggerFactory));
    ON_CALL(core, getExifReaderFactory).WillByDefault(ReturnRef(exifFactory));
    ON_CALL(loggerFactory, get(An<const QString &>())).WillByDefault(Invoke([]{ return std::make_unique<EmptyLogger>(); }));

    const auto info = MediaInformation(&core).getInformation(QString::fromStdString(std::string(path)));

    // valdiate creation time
    if (creationTime.isValid())
    {
        ASSERT_TRUE(info.common.creationTime);

        const auto gotTime = *info.common.creationTime;
        EXPECT_EQ(gotTime, creationTime);
    }

    // valdiate resolution
    ASSERT_TRUE(info.common.dimension);
    const auto gotDimension = *info.common.dimension;
    EXPECT_EQ(gotDimension, dimension);

    // validate duration
    ASSERT_TRUE(std::holds_alternative<VideoFile>(info.details));
    const auto videoData = std::get<VideoFile>(info.details);
    EXPECT_EQ(videoData.duration, duration);
}
