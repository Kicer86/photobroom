
#include <gmock/gmock.h>

#include "database_tools/implementation/photo_info_updater.hpp"
#include "unit_tests_utils/empty_logger.hpp"
#include "unit_tests_utils/fake_task_executor.hpp"
#include "unit_tests_utils/mock_backend.hpp"
#include "unit_tests_utils/mock_core_factory_accessor.hpp"
#include "unit_tests_utils/mock_configuration.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/mock_exif_reader_factory.hpp"
#include "unit_tests_utils/mock_exif_reader.hpp"
#include "unit_tests_utils/mock_logger_factory.hpp"
#include "unit_tests_utils/printers.hpp"


using testing::_;
using testing::An;
using testing::Invoke;
using testing::ReturnRef;
using testing::NiceMock;


TEST(PhotoInfoUpdaterTest, exifUpdate)
{
    FakeTaskExecutor taskExecutor;
    NiceMock<MockBackend> backend;
    NiceMock<ExifReaderFactoryMock> exifFactoryMock;
    NiceMock<MockExifReader> exifReader;
    NiceMock<ILoggerFactoryMock> loggerFactoryMock;
    NiceMock<IConfigurationMock> configurationMock;
    NiceMock<ICoreFactoryAccessorMock> coreFactory;
    NiceMock<MockDatabase> db;

    ON_CALL(coreFactory, getExifReaderFactory).WillByDefault(ReturnRef(exifFactoryMock));
    ON_CALL(coreFactory, getConfiguration).WillByDefault(ReturnRef(configurationMock));
    ON_CALL(coreFactory, getLoggerFactory).WillByDefault(ReturnRef(loggerFactoryMock));
    ON_CALL(coreFactory, getTaskExecutor).WillByDefault(ReturnRef(taskExecutor));
    ON_CALL(exifFactoryMock, get).WillByDefault(ReturnRef(exifReader));
    ON_CALL(loggerFactoryMock, get(An<const QString &>())).WillByDefault(Invoke([](const auto &)
    {
        return std::make_unique<EmptyLogger>();
    }));

    ON_CALL(db, execute).WillByDefault(Invoke([&backend](std::unique_ptr<Database::IDatabase::ITask>&& task)
    {
        task->run(backend);
    }));

    // orignal state of photo
    Photo::Data photo;
    photo.id = Photo::Id(123);
    photo.flags = { {Photo::FlagsE::StagingArea, 1}, {Photo::FlagsE::Sha256Loaded, 2} };
    photo.tags = { {TagTypes::Event, TagValue::fromType<TagTypes::Event>("qweasd")}, {TagTypes::Rating, 5} };

    // expected state after calling updater
    Photo::Data newPhotoData(photo);
    newPhotoData.flags[Photo::FlagsE::ExifLoaded] = 1;

    PhotoInfoUpdater updater(&coreFactory, db);

    Photo::SharedData sharedData = std::make_shared<Photo::SafeData>(photo);
    updater.updateTags(sharedData);

    const auto lockedData = sharedData->lock();
    EXPECT_EQ(*lockedData, newPhotoData);
}
