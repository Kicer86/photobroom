
#include <gtest/gtest.h>

#include "media_information.hpp"
#include "unit_tests_utils/mock_configuration.hpp"
#include "unit_tests_utils/mock_core_factory_accessor.hpp"
#include "unit_tests_utils/mock_logger_factory.hpp"
#include "unit_tests_utils/mock_exif_reader_factory.hpp"
#include "unit_tests_utils/empty_logger.hpp"


using testing::An;
using testing::Invoke;
using testing::NiceMock;
using testing::ReturnRef;


struct VideoMediaInformationTestParam
{
    std::string_view path;
    std::chrono::system_clock::time_point creation_time;
};

class VideoMediaInformationTest: public testing::TestWithParam<VideoMediaInformationTestParam> {};

INSTANTIATE_TEST_SUITE_P(
    SampleVideos, VideoMediaInformationTest,
    testing::Values
    (
        VideoMediaInformationTestParam
        {
            "Atoms - 8579.mp4",
            std::chrono::system_clock::from_time_t(0)
        }
    ));


TEST_P(VideoMediaInformationTest, validateMetadata)
{
    const auto& [path, creation_time] = GetParam();
    NiceMock<IConfigurationMock> config;
    NiceMock<ICoreFactoryAccessorMock> core;
    NiceMock<ILoggerFactoryMock> loggerFactory;
    ExifReaderFactoryMock exifFactory;

    ON_CALL(core, getConfiguration).WillByDefault(ReturnRef(config));
    ON_CALL(core, getLoggerFactory).WillByDefault(ReturnRef(loggerFactory));
    ON_CALL(core, getExifReaderFactory).WillByDefault(ReturnRef(exifFactory));
    ON_CALL(loggerFactory, get(An<const QString &>())).WillByDefault(Invoke([]{ return std::make_unique<EmptyLogger>(); }));

    const auto info = MediaInformation(&core).getInformation(QString::fromStdString(std::string(path)));

    ASSERT_TRUE(info.common.creationTime);

    const auto gotTime = *info.common.creationTime;
    const auto creationTimeT = std::chrono::system_clock::to_time_t(creation_time);
    const auto creationTimeQ = QDateTime::fromSecsSinceEpoch(creationTimeT);
    EXPECT_EQ(gotTime, creationTimeQ);
}
