
#include <random>

#include <QDate>
#include <QTime>

#include <unit_tests_utils/empty_logger.hpp>
#include <unit_tests_utils/mock_backend.hpp>
#include <unit_tests_utils/mock_exif_reader.hpp>
#include <unit_tests_utils/mock_photo_operator.hpp>

#include "backends/memory_backend/memory_backend.hpp"
#include "database_tools/json_to_backend.hpp"
#include "database_tools/series_detector.hpp"
#include "unit_tests_utils/db_for_series_detection.json.hpp"
#include "unit_tests_utils/mock_database.hpp"


using testing::Invoke;
using testing::NiceMock;
using testing::Return;
using testing::ReturnRef;
using testing::_;


class SeriesDetectorTest: public testing::Test
{
    public:
        NiceMock<MockDatabase> db;
        NiceMock<MockBackend> backend;
        EmptyLogger logger;

        SeriesDetectorTest()
        {
            ON_CALL(db, execute(_)).WillByDefault(Invoke([this](const auto& task)
            {
                task->run(backend);
            }));
        }
};


TEST_F(SeriesDetectorTest, constructor)
{
    EXPECT_NO_THROW({
        MockExifReader exif;

        SeriesDetector sd(logger, db, exif);
    });
}


TEST_F(SeriesDetectorTest, animationDetectionScenario1)
{
    NiceMock<MockExifReader> exif;
    NiceMock<PhotoOperatorMock> photoOperator;

    ON_CALL(backend, photoOperator()).WillByDefault(ReturnRef(photoOperator));

    // Mock 6 photos
    // divideded into two groups.
    // Each group has SequenceNumber in exif from 1 to 3
    // All photos are made at different time (1 second difference between photos)
    std::vector<Photo::Id> all_photos =
    {
        Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4), Photo::Id(5), Photo::Id(6)
    };

    ON_CALL(photoOperator, onPhotos(_, Database::Action(Database::Actions::Sort(Database::Actions::Sort::By::Timestamp)))).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhotoDelta(_, _)).WillByDefault(Invoke([](const Photo::Id& id, const auto &) -> Photo::DataDelta
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1.jpeg").arg(id.value());                                    // add id to path so exif mock can use it for data mocking
        data.tags.emplace(Tag::Types::Date, QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(Tag::Types::Time, QTime::fromString(QString("12.00.%1").arg(id.value()), "hh.mm.s"));  // simulate different time - use id as second

        return Photo::DataDelta(data);
    }));

    // return sequence number basing on file name (file name contains photo id)
    ON_CALL(exif, get(_, IExifReader::TagType::SequenceNumber)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        QString id_str = pathSplitted.back();
        id_str.remove(".jpeg");
        const int id = id_str.toInt();

        result = std::any( (id - 1) % 3 + 1);   // id:1 -> 1, id:2 -> 2, id:3 -> 3, id:4 -> 1 ...

        return result;
    }));

    const SeriesDetector sd(logger, db, exif);
    const std::vector<GroupCandidate> groupCanditates = sd.listCandidates();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::Animation);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::Animation);
}


TEST_F(SeriesDetectorTest, animationDetectionScenario2)
{
    NiceMock<MockExifReader> exif;
    NiceMock<PhotoOperatorMock> photoOperator;

    ON_CALL(backend, photoOperator()).WillByDefault(ReturnRef(photoOperator));

    // Mock 6 photos
    // divideded into two groups.
    // Each group has SequenceNumber in exif from 1 to 3
    // Photos within a group have the same time of take
    std::vector<Photo::Id> all_photos =
    {
        Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4), Photo::Id(5), Photo::Id(6)
    };

    ON_CALL(photoOperator, onPhotos(_, Database::Action(Database::Actions::Sort(Database::Actions::Sort::By::Timestamp)))).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhotoDelta(_, _)).WillByDefault(Invoke([](const Photo::Id& id, const auto &) -> Photo::DataDelta
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1.jpeg").arg(id.value());        // add id to path so exif mock can use it for data mocking
        data.tags.emplace(Tag::Types::Date, QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(Tag::Types::Time, QTime::fromString(QString("12.00.%1").arg( (id.value() - 1) / 3), "hh.mm.s"));  // simulate same time within a group

        return Photo::DataDelta(data);
    }));

    // return sequence number basing on file name (file name contains photo id)
    ON_CALL(exif, get(_, IExifReader::TagType::SequenceNumber)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        QString id_str = pathSplitted.back();
        id_str.remove(".jpeg");
        const int id = id_str.toInt();

        result = std::any( (id - 1) % 3 + 1);   // id:1 -> 1, id:2 -> 2, id:3 -> 3, id:4 -> 1 ...

        return result;
    }));

    const SeriesDetector sd(logger, db, exif);
    const std::vector<GroupCandidate> groupCanditates = sd.listCandidates();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::Animation);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::Animation);
}


TEST_F(SeriesDetectorTest, animationDetectionScenario3)
{
    NiceMock<MockExifReader> exif;
    NiceMock<PhotoOperatorMock> photoOperator;

    ON_CALL(backend, photoOperator()).WillByDefault(ReturnRef(photoOperator));

    // Mock 6 photos
    // divideded into two groups.
    // Each group has SequenceNumber in exif from 1 to 3
    // Photos within a group have the same time of take
    // All photos have the same, but non zero exposure
    std::vector<Photo::Id> all_photos =
    {
        Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4), Photo::Id(5), Photo::Id(6)
    };

    ON_CALL(photoOperator, onPhotos(_, Database::Action(Database::Actions::Sort(Database::Actions::Sort::By::Timestamp)))).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhotoDelta(_, _)).WillByDefault(Invoke([](const Photo::Id& id, const auto &) -> Photo::DataDelta
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1.jpeg").arg(id.value());        // add id to path so exif mock can use it for data mocking
        data.tags.emplace(Tag::Types::Date, QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(Tag::Types::Time, QTime::fromString(QString("12.00.%1").arg( (id.value() - 1) / 3), "hh.mm.s"));  // simulate same time within a group

        return Photo::DataDelta(data);
    }));

    // return sequence number basing on file name (file name contains photo id)
    ON_CALL(exif, get(_, IExifReader::TagType::SequenceNumber)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        QString id_str = pathSplitted.back();
        id_str.remove(".jpeg");
        const int id = id_str.toInt();

        result = std::any( (id - 1) % 3 + 1);   // id:1 -> 1, id:2 -> 2, id:3 -> 3, id:4 -> 1 ...

        return result;
    }));

    ON_CALL(exif, get(_, IExifReader::TagType::Exposure)).WillByDefault(Return(-1.f));

    const SeriesDetector sd(logger, db, exif);
    const std::vector<GroupCandidate> groupCanditates = sd.listCandidates();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::Animation);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::Animation);
}


TEST_F(SeriesDetectorTest, smartphoneSeries)
{
    NiceMock<MockExifReader> exif;
    NiceMock<PhotoOperatorMock> photoOperator;

    ON_CALL(backend, photoOperator()).WillByDefault(ReturnRef(photoOperator));

    // Mock 6 photos
    // divideded into two groups.
    // Each group has BURSTXXX in filename from 1 to 3
    // All photos are made at different time (1 second difference between photos)
    std::vector<Photo::Id> all_photos =
    {
        Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4), Photo::Id(5), Photo::Id(6)
    };

    ON_CALL(photoOperator, onPhotos(_, Database::Action(Database::Actions::Sort(Database::Actions::Sort::By::Timestamp)))).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhotoDelta(_, _)).WillByDefault(Invoke([](const Photo::Id& id, const auto &) -> Photo::DataDelta
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path_BURST%1.jpeg").arg(id.value() % 3 + 1);                                    // add id to path so exif mock can use it for data mocking
        data.tags.emplace(Tag::Types::Date, QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(Tag::Types::Time, QTime::fromString(QString("12.00.%1").arg(id.value()), "hh.mm.s"));  // simulate different time - use id as second

        return Photo::DataDelta(data);
    }));

    const SeriesDetector sd(logger, db, exif);
    const std::vector<GroupCandidate> groupCanditates = sd.listCandidates();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::Animation);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::Animation);
}


TEST_F(SeriesDetectorTest, HDRDetectionScenario1)
{
    NiceMock<MockExifReader> exif;
    NiceMock<PhotoOperatorMock> photoOperator;

    ON_CALL(backend, photoOperator()).WillByDefault(ReturnRef(photoOperator));

    // Mock 6 photos
    // divideded into two groups.
    // Each group has SequenceNumber in exif from 1 to 3
    // Photos within a group have the same time of take
    // in both groups photos have different exposure level.

    // photos within group have same timestamp.
    // returning them in pseudo random order
    // so sequence numbers will be mixed
    std::vector<Photo::Id> all_photos =
    {
        // first group
        Photo::Id(2), Photo::Id(3), Photo::Id(1),

        // second group
        Photo::Id(6), Photo::Id(4), Photo::Id(5)
    };

    ON_CALL(photoOperator, onPhotos(_, Database::Action(Database::Actions::Sort(Database::Actions::Sort::By::Timestamp)) )).WillByDefault(Return(all_photos));
    ON_CALL(backend, getPhotoDelta(_, _)).WillByDefault(Invoke([](const Photo::Id& id, const auto &) -> Photo::DataDelta
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1.jpeg").arg(id.value());        // add id to path so exif mock can use it for data mocking
        data.tags.emplace(Tag::Types::Date, QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(Tag::Types::Time, QTime::fromString(QString("12.00.%1").arg( (id.value() - 1) / 3), "hh.mm.s"));  // simulate same time within a group

        return Photo::DataDelta(data);
    }));

    // return sequence number basing on file name (file name contains photo id)
    ON_CALL(exif, get(_, IExifReader::TagType::SequenceNumber)).WillByDefault(Invoke([](const QString& path, IExifReader::TagType) -> std::optional<std::any>
    {
        std::optional<std::any> result;

        const QStringList pathSplitted = path.split(" ");
        assert(pathSplitted.size() == 2);

        QString id_str = pathSplitted.back();
        id_str.remove(".jpeg");
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

        QString id_str = pathSplitted.back();
        id_str.remove(".jpeg");
        const int id = id_str.toInt();

        result = std::any( float((id - 1) % 3 - 1) );   // id:1 -> -1.0, id:2 -> 0.0, id:3 -> 1.0, id:4 -> -1.0 ...

        return result;
    }));

    const SeriesDetector sd(logger, db, exif);
    const std::vector<GroupCandidate> groupCanditates = sd.listCandidates();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 3);
    ASSERT_EQ(groupCanditates.back().members.size(), 3);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::HDR);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::HDR);
}


TEST_F(SeriesDetectorTest, PhotosTakenOneByOne)
{
    NiceMock<MockExifReader> exif;
    Database::MemoryBackend mem_backend;
    Database::JsonToBackend jsonReader(mem_backend);

    jsonReader.append(SeriesDB::db);

    NiceMock<MockDatabase> mem_db;

    ON_CALL(mem_db, execute(_)).WillByDefault(Invoke([&mem_backend](const auto& task)
    {
        task->run(mem_backend);
    }));

    const SeriesDetector sd(logger, mem_db, exif);
    const std::vector<GroupCandidate> groupCanditates = sd.listCandidates();

    ASSERT_EQ(groupCanditates.size(), 2);
    ASSERT_EQ(groupCanditates.front().members.size(), 6);
    ASSERT_EQ(groupCanditates.back().members.size(), 5);
    EXPECT_EQ(groupCanditates.front().type, Group::Type::Generic);
    EXPECT_EQ(groupCanditates.back().type, Group::Type::Generic);
}


TEST_F(SeriesDetectorTest, Complexity)
{
    NiceMock<MockExifReader> exif;
    NiceMock<PhotoOperatorMock> photoOperator;

    ON_CALL(backend, photoOperator()).WillByDefault(ReturnRef(photoOperator));

    // Mock some photos
    std::vector<Photo::Id> all_photos;

    for(int i = 1; i <= 50; i++)
        all_photos.push_back(Photo::Id(i));

    ON_CALL(photoOperator, onPhotos(_, Database::Action(Database::Actions::Sort(Database::Actions::Sort::By::Timestamp)))).WillByDefault(Return(all_photos));

    EXPECT_CALL(backend, getPhotoDelta(_, _)).Times(50).WillRepeatedly(Invoke([](const Photo::Id& id, const auto &) -> Photo::DataDelta
    {
        Photo::Data data;
        data.id = id;
        data.path = QString("path: %1.jpeg").arg(id.value());        // add id to path so exif mock can use it for data mocking
        data.tags.emplace(Tag::Types::Date, QDate::fromString("2000.12.01", "yyyy.MM.dd"));
        data.tags.emplace(Tag::Types::Time, QTime::fromString(QString("12.%1.00").arg(id.value()), "hh.m.ss"));  // simulate different time - use id as minute

        return Photo::DataDelta(data);
    }));

    const SeriesDetector sd(logger, db, exif);
    const std::vector<GroupCandidate> groupCanditates = sd.listCandidates();
}
