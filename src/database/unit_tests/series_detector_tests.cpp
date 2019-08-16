
#include <random>

#include <QDate>
#include <QTime>

#include <unit_tests_utils/mock_backend.hpp>
#include <unit_tests_utils/mock_exif_reader.hpp>

#include "database_tools/series_detector.hpp"

using testing::Invoke;
using testing::NiceMock;
using testing::Return;
using testing::_;


TEST(SeriesDetectorTest, constructor)
{
    EXPECT_NO_THROW({
        MockBackend backend;
        MockExifReader exif;

        SeriesDetector sd(&backend, &exif);
    });
}


TEST(SeriesDetectorTest, animationDetectionScenario1)
{
    NiceMock<MockBackend> backend;
    NiceMock<MockExifReader> exif;

    // Mock 6 photos
    // divideded into two groups.
    // Each group has SequenceNumber in exif from 1 to 3
    // All photos are made at different time (1 second difference between photos)
    std::vector<Photo::Id> all_photos =
    {
        Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4), Photo::Id(5), Photo::Id(6)
    };

    // shuffle photos so they come in undefined order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_photos.begin(), all_photos.end(), g);

    ON_CALL(backend, getPhotos(_)).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhoto(_)).WillByDefault(Invoke([](const Photo::Id& id) -> Photo::Data
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1").arg(id);        // add id to path so exif mock can use it for data mocking
        data.tags.emplace(TagNameInfo(BaseTagsList::Date), QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(TagNameInfo(BaseTagsList::Time), QTime::fromString(QString("12.00.%1").arg(id), "hh.mm.s"));  // simulate different time - use id as second

        return data;
    }));

    // return sequence number basing on file name (file name contains photo id)
    ON_CALL(exif, get(_, IExifReader::TagType::SequenceNumber)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        const QString id_str = pathSplitted.back();
        const int id = id_str.toInt();

        result = std::any( (id - 1) % 3 + 1);   // id:1 -> 1, id:2 -> 2, id:3 -> 3, id:4 -> 1 ...

        return result;
    }));

    const SeriesDetector sd(&backend, &exif);
    const std::vector<SeriesDetector::Detection> groupCanditates = sd.listDetections();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::Animation);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::Animation);
}


TEST(SeriesDetectorTest, animationDetectionScenario2)
{
    NiceMock<MockBackend> backend;
    NiceMock<MockExifReader> exif;

    // Mock 6 photos
    // divideded into two groups.
    // Each group has SequenceNumber in exif from 1 to 3
    // Photos within a group have the same time of take
    std::vector<Photo::Id> all_photos =
    {
        Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4), Photo::Id(5), Photo::Id(6)
    };

    // shuffle photos so they come in undefined order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_photos.begin(), all_photos.end(), g);

    ON_CALL(backend, getPhotos(_)).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhoto(_)).WillByDefault(Invoke([](const Photo::Id& id) -> Photo::Data
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1").arg(id);        // add id to path so exif mock can use it for data mocking
        data.tags.emplace(TagNameInfo(BaseTagsList::Date), QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(TagNameInfo(BaseTagsList::Time), QTime::fromString(QString("12.00.%1").arg( (id - 1) / 3), "hh.mm.s"));  // simulate same time within a group

        return data;
    }));

    // return sequence number basing on file name (file name contains photo id)
    ON_CALL(exif, get(_, IExifReader::TagType::SequenceNumber)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        const QString id_str = pathSplitted.back();
        const int id = id_str.toInt();

        result = std::any( (id - 1) % 3 + 1);   // id:1 -> 1, id:2 -> 2, id:3 -> 3, id:4 -> 1 ...

        return result;
    }));

    const SeriesDetector sd(&backend, &exif);
    const std::vector<SeriesDetector::Detection> groupCanditates = sd.listDetections();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::Animation);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::Animation);
}


TEST(SeriesDetectorTest, animationDetectionScenario3)
{
    NiceMock<MockBackend> backend;
    NiceMock<MockExifReader> exif;

    // Mock 6 photos
    // divideded into two groups.
    // Each group has SequenceNumber in exif from 1 to 3
    // Photos within a group have the same time of take
    // All photos have the same, but non zero exposure
    std::vector<Photo::Id> all_photos =
    {
        Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4), Photo::Id(5), Photo::Id(6)
    };

    // shuffle photos so they come in undefined order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_photos.begin(), all_photos.end(), g);

    ON_CALL(backend, getPhotos(_)).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhoto(_)).WillByDefault(Invoke([](const Photo::Id& id) -> Photo::Data
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1").arg(id);        // add id to path so exif mock can use it for data mocking
        data.tags.emplace(TagNameInfo(BaseTagsList::Date), QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(TagNameInfo(BaseTagsList::Time), QTime::fromString(QString("12.00.%1").arg( (id - 1) / 3), "hh.mm.s"));  // simulate same time within a group

        return data;
    }));

    // return sequence number basing on file name (file name contains photo id)
    ON_CALL(exif, get(_, IExifReader::TagType::SequenceNumber)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        const QString id_str = pathSplitted.back();
        const int id = id_str.toInt();

        result = std::any( (id - 1) % 3 + 1);   // id:1 -> 1, id:2 -> 2, id:3 -> 3, id:4 -> 1 ...

        return result;
    }));

    ON_CALL(exif, get(_, IExifReader::TagType::Exposure)).WillByDefault(Return(-1.f));

    const SeriesDetector sd(&backend, &exif);
    const std::vector<SeriesDetector::Detection> groupCanditates = sd.listDetections();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::Animation);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::Animation);
}


TEST(SeriesDetectorTest, HDRDetectionScenario1)
{
    NiceMock<MockBackend> backend;
    NiceMock<MockExifReader> exif;

    // Mock 6 photos
    // divideded into two groups.
    // Each group has SequenceNumber in exif from 1 to 3
    // Photos within a group have the same time of take
    // in both groups photos have different exposure level.
    std::vector<Photo::Id> all_photos =
    {
        Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4), Photo::Id(5), Photo::Id(6)
    };

    // shuffle photos so they come in undefined order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_photos.begin(), all_photos.end(), g);

    ON_CALL(backend, getPhotos(_)).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhoto(_)).WillByDefault(Invoke([](const Photo::Id& id) -> Photo::Data
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1").arg(id);        // add id to path so exif mock can use it for data mocking
        data.tags.emplace(TagNameInfo(BaseTagsList::Date), QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(TagNameInfo(BaseTagsList::Time), QTime::fromString(QString("12.00.%1").arg( (id - 1) / 3), "hh.mm.s"));  // simulate same time within a group

        return data;
    }));

    // return sequence number basing on file name (file name contains photo id)
    ON_CALL(exif, get(_, IExifReader::TagType::SequenceNumber)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        const QString id_str = pathSplitted.back();
        const int id = id_str.toInt();

        result = std::any( (id - 1) % 3 + 1);   // id:1 -> 1, id:2 -> 2, id:3 -> 3, id:4 -> 1 ...

        return result;
    }));

    // return EV basing on file name
    ON_CALL(exif, get(_, IExifReader::TagType::Exposure)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        const QString id_str = pathSplitted.back();
        const int id = id_str.toInt();

        result = std::any( float((id - 1) % 3 - 1) );   // id:1 -> -1.0, id:2 -> 0.0, id:3 -> 1.0, id:4 -> -1.0 ...

        return result;
    }));

    const SeriesDetector sd(&backend, &exif);
    const std::vector<SeriesDetector::Detection> groupCanditates = sd.listDetections();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::HDR);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::HDR);
}
