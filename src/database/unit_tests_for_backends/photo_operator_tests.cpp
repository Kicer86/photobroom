
#include <core/utils.hpp>

#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/sample_db.json.hpp"
#include "unit_tests_utils/sample_db2.json.hpp"
#include "unit_tests_utils/phash_db.json.hpp"
#include "unit_tests_utils/rich_db.json.hpp"
#include "database/ut_printers.hpp"

#include "common.hpp"


using testing::Contains;
using testing::ElementsAre;
using testing::UnorderedElementsAreArray;


MATCHER_P(IsPhotoWithPath, _path, "") {
    return arg.template get<Photo::Field::Path>() == _path;
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

    std::vector<Photo::ExplicitDelta<Photo::Field::Path>> photo_data;
    photo_data.push_back(this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[0]));
    photo_data.push_back(this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[1]));
    photo_data.push_back(this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[2]));

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

    std::vector<Photo::ExplicitDelta<Photo::Field::Path>> photo_data;
    photo_data.push_back(this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[0]));
    photo_data.push_back(this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[1]));
    photo_data.push_back(this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[2]));

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
        const auto photo_data = this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[p]);
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

    std::vector<Photo::ExplicitDelta<Photo::Field::PHash>> photos;
    for (const auto& id: ids)
        photos.push_back(this->m_backend->template getPhotoDelta<Photo::Field::PHash>(id));

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
        photos.push_back(this->m_backend->template getPhotoDelta<Photo::Field::PHash>(id));

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
    const auto readData = this->m_backend->template getPhotoDelta<Photo::Field::Path>(id);
    const auto& readDataPath = readData.template get<Photo::Field::Path>();
    EXPECT_EQ(readDataPath, path);
}


TYPED_TEST(PhotoOperatorTest, fetchDeltasForAllPhotos)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db2);

    const auto photos = this->m_backend->photoOperator().fetchData(Database::EmptyFilter());

    ASSERT_EQ(photos.size(), 21);
}


TYPED_TEST(PhotoOperatorTest, fetchDeltasForSomePhotos)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db2);

    const auto photos = this->m_backend->photoOperator().fetchData(Database::FilterPhotosWithTag(Tag::Types::Time, QTime(10, 0, 0)));

    ASSERT_EQ(photos.size(), 7);
}


TYPED_TEST(PhotoOperatorTest, compareCompletness)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(RichDB::db1);

    const auto filter = Database::FilterPhotosWithTag(Tag::Types::Time, QTime(10, 0, 0));
    const auto fetchedPhotos = this->m_backend->photoOperator().fetchData(filter);
    const auto ids = this->m_backend->photoOperator().getPhotos(filter);

    std::vector<Photo::DataDelta> gotPhotos;
    for (const auto id: ids)
    {
        const auto delta = this->m_backend->getPhotoDelta(id);
        gotPhotos.push_back(delta);
    }

    // both methods should return the same set of data (possibily in different order)
    EXPECT_THAT(fetchedPhotos, UnorderedElementsAreArray(gotPhotos));
}


TYPED_TEST(PhotoOperatorTest, fetchPhotoParts)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(RichDB::db1);

    for_each<Photo::Field>([&]<auto field>()
    {
        std::vector<Photo::ExplicitDelta<field>> fromBackendPhotos;

        const auto ids = this->m_backend->photoOperator().getPhotos({});
        for (const auto id: ids)
        {
            Photo::ExplicitDelta<field> fromBackendPhoto = this->m_backend->template getPhotoDelta<field>(id);
            fromBackendPhotos.push_back(fromBackendPhoto);
        }

        const auto fromOperatorPhotos = this->m_backend->photoOperator().template fetchData<field>({});

        EXPECT_THAT(fromBackendPhotos, UnorderedElementsAreArray(fromOperatorPhotos));
    });
}


TYPED_TEST(PhotoOperatorTest, photosCountMatchesGetPhotos)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db2);

    const auto filter = Database::FilterPhotosWithTag(Tag::Types::Time, QTime(10, 0, 0));

    const auto count = this->m_backend->getPhotosCount(filter);
    const auto ids = this->m_backend->photoOperator().getPhotos(filter);

    EXPECT_EQ(count, static_cast<int>(ids.size()));
}


TYPED_TEST(PhotoOperatorTest, deletionMarksAndNotifies)
{
    Photo::DataDelta pd1, pd2;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");
    pd2.insert<Photo::Field::Path>("photo2.jpeg");

    std::vector<Photo::DataDelta> photos{pd1, pd2};
    this->m_backend->addPhotos(photos);

    const auto id1 = photos[0].getId();
    const auto id2 = photos[1].getId();

    QSignalSpy spy(this->m_backend.get(), &Database::IBackend::photosRemoved);

    this->m_backend->photoOperator().removePhoto(id1);
    ASSERT_EQ(spy.size(), 1);
    EXPECT_THAT(spy.at(0).at(0).value<std::vector<Photo::Id>>(), ElementsAre(id1));
    auto state = this->m_backend->get(id1, Database::CommonGeneralFlags::State);
    ASSERT_TRUE(state.has_value());
    EXPECT_EQ(*state, static_cast<int>(Database::CommonGeneralFlags::StateType::Delete));

    spy.clear();

    Database::FilterPhotosWithId filter; filter.filter = id2;
    this->m_backend->photoOperator().removePhotos(filter);
    ASSERT_EQ(spy.size(), 1);
    EXPECT_THAT(spy.at(0).at(0).value<std::vector<Photo::Id>>(), ElementsAre(id2));
    state = this->m_backend->get(id2, Database::CommonGeneralFlags::State);
    ASSERT_TRUE(state.has_value());
    EXPECT_EQ(*state, static_cast<int>(Database::CommonGeneralFlags::StateType::Delete));
}
