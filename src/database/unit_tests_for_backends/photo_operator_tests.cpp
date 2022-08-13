
#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/sample_db.json.hpp"
#include "unit_tests_utils/sample_db2.json.hpp"
#include "unit_tests_utils/phash_db.json.hpp"

#include "common.hpp"

using testing::Contains;
using testing::ElementsAre;


MATCHER_P(IsPhotoWithPath, _path, "") {
    return arg.path == _path;
}

template<typename T>
struct PhotoOperatorTest: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(PhotoOperatorTest, BackendTypes);


TYPED_TEST(PhotoOperatorTest, gettingAllPhotos)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    const auto photos = this->m_backend->photoOperator().getPhotos({});

    ASSERT_EQ(photos.size(), 3);

    std::vector<Photo::Data> photo_data;
    photo_data.push_back(this->m_backend->getPhoto(photos[0]));
    photo_data.push_back(this->m_backend->getPhoto(photos[1]));
    photo_data.push_back(this->m_backend->getPhoto(photos[2]));

    EXPECT_THAT(photo_data, Contains(IsPhotoWithPath("/some/path1.jpeg")));
    EXPECT_THAT(photo_data, Contains(IsPhotoWithPath("/some/path2.jpeg")));
    EXPECT_THAT(photo_data, Contains(IsPhotoWithPath("/some/path3.jpeg")));
}


TYPED_TEST(PhotoOperatorTest, sortingByTagActionOnPhotos)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    Database::Actions::SortByTag sort(Tag::Types::Event, Qt::AscendingOrder);
    const auto photos = this->m_backend->photoOperator().onPhotos({}, {sort});

    ASSERT_EQ(photos.size(), 3);

    std::vector<Photo::Data> photo_data;
    photo_data.push_back(this->m_backend->getPhoto(photos[0]));
    photo_data.push_back(this->m_backend->getPhoto(photos[1]));
    photo_data.push_back(this->m_backend->getPhoto(photos[2]));

    EXPECT_THAT(photo_data[0], IsPhotoWithPath("/some/path2.jpeg"));
    EXPECT_THAT(photo_data[1], IsPhotoWithPath("/some/path3.jpeg"));
    EXPECT_THAT(photo_data[2], IsPhotoWithPath("/some/path1.jpeg"));
}


TYPED_TEST(PhotoOperatorTest, sortingByTimestampActionOnPhotos)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db2);

    Database::Actions::Sort sort(Database::Actions::Sort::By::Timestamp, Qt::DescendingOrder);
    const auto photos = this->m_backend->photoOperator().onPhotos({}, {sort});

    ASSERT_EQ(photos.size(), 21);

    for (int idx = 21, p = 0; p < 21; idx--, p++)
    {
        Photo::Data photo_data;
        photo_data = this->m_backend->getPhoto(photos[p]);

        const QString expected_path = QString("/some/path%1.jpeg").arg(idx);
        EXPECT_THAT(photo_data, IsPhotoWithPath(expected_path));
    }
}


TYPED_TEST(PhotoOperatorTest, sortingByPHash)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(PHashDB::db);

    Database::Actions::Sort sort(Database::Actions::Sort::By::PHash);
    const auto ids = this->m_backend->photoOperator().onPhotos({}, {sort});

    ASSERT_EQ(ids.size(), 12);

    std::vector<Photo::DataDelta> photos;
    for (const auto& id: ids)
        photos.push_back(this->m_backend->getPhotoDelta(id, {Photo::Field::PHash}));

    std::vector<int> phashes;
    std::transform(photos.begin(), photos.end(), std::back_inserter(phashes), [](const Photo::DataDelta& data) { return data.get<Photo::Field::PHash>().value(); });

    EXPECT_THAT(phashes, ElementsAre(1, 1, 2, 2, 3, 3, 3, 4, 5, 6, 8, 16));
}


TYPED_TEST(PhotoOperatorTest, sortingByPHashReversed)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(PHashDB::db);

    Database::Actions::Sort sort(Database::Actions::Sort::By::PHash, Qt::DescendingOrder);
    const auto ids = this->m_backend->photoOperator().onPhotos({}, {sort});

    ASSERT_EQ(ids.size(), 12);

    std::vector<Photo::DataDelta> photos;
    for (const auto& id: ids)
        photos.push_back(this->m_backend->getPhotoDelta(id, {Photo::Field::PHash}));

    std::vector<int> phashes;
    std::transform(photos.begin(), photos.end(), std::back_inserter(phashes), [](const Photo::DataDelta& data) { return data.get<Photo::Field::PHash>().value(); });

    EXPECT_THAT(phashes, ElementsAre(16, 8, 6, 5, 4, 3, 3, 3, 2, 2, 1, 1));
}


TYPED_TEST(PhotoOperatorTest, sortingByID)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(PHashDB::db);

    Database::Actions::Sort sort(Database::Actions::Sort::By::ID);
    const auto ids = this->m_backend->photoOperator().onPhotos({}, {sort});

    ASSERT_EQ(ids.size(), 12);

    EXPECT_TRUE(std::is_sorted(ids.begin(), ids.end()));
}


TYPED_TEST(PhotoOperatorTest, sortingByIDRev)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(PHashDB::db);

    Database::Actions::Sort sort(Database::Actions::Sort::By::ID, Qt::DescendingOrder);
    const auto ids = this->m_backend->photoOperator().onPhotos({}, {sort});

    ASSERT_EQ(ids.size(), 12);

    EXPECT_TRUE(std::is_sorted(ids.rbegin(), ids.rend()));
}


TYPED_TEST(PhotoOperatorTest, removal)
{
    // insert photo
    Photo::DataDelta data;
    const auto path = QStringLiteral("some path");
    data.insert<Photo::Field::Path>(path);
    std::vector<Photo::DataDelta> photos = {data};
    this->m_backend->addPhotos(photos);

    // delete photo
    const Photo::Id id = photos.front().getId();
    this->m_backend->photoOperator().removePhoto(id);

    // Photo should be still accessible but marked as to be deleted.
    // Reason for this behavior is that Photo::Id may be used by many clients.
    // Some may ask Photo::DataDelta for it while photo is being deleted.
    // It is not convenient to protect them all against null result.
    // Instead db should mark such photos and delete them later (possibly on db close).
    const Photo::DataDelta readData = this->m_backend->getPhotoDelta(id, {Photo::Field::Path});    // TODO: for some reason Photo::DataDelta cannot be replaced with auto. gcc 12.1.1 bug?
    const auto& readDataPath = readData.get<Photo::Field::Path>();
    EXPECT_EQ(readDataPath, path);


}

