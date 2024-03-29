
#include <QSignalSpy>

#include "common.hpp"
#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/sample_db.json.hpp"
#include "unit_tests_utils/sample_db_with_groups.json.hpp"

using testing::Eq;

template<typename T>
struct TransactionAccumulationsTests: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(TransactionAccumulationsTests, BackendTypes);

TYPED_TEST(TransactionAccumulationsTests, newPhotos)
{
    Photo::DataDelta photo1, photo2;
    photo1.insert<Photo::Field::Path>("/path/photo.jpeg");
    photo2.insert<Photo::Field::Path>("/path/photo.jpeg");

    std::vector photos1{photo1};
    std::vector photos2{photo2};

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosAdded);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->addPhotos(photos1);
        this->m_backend->addPhotos(photos2);
    }

    // insertions should be accumulated into one notification
    EXPECT_EQ(notifications.count(), 1);
}


TYPED_TEST(TransactionAccumulationsTests, newPhotosAborted)
{
    Photo::DataDelta photo1, photo2;
    photo1.insert<Photo::Field::Path>("/path/photo.jpeg");
    photo2.insert<Photo::Field::Path>("/path/photo.jpeg");

    std::vector photos1{photo1};
    std::vector photos2{photo2};

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosAdded);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->addPhotos(photos1);
        this->m_backend->addPhotos(photos2);
        transaction->abort();
    }

    // transaction aborted - no notifications
    EXPECT_EQ(notifications.count(), 0);
}


TYPED_TEST(TransactionAccumulationsTests, newPhotosAbortedInMiddle)
{
    Photo::DataDelta photo1, photo2;
    photo1.insert<Photo::Field::Path>("/path/photo.jpeg");
    photo2.insert<Photo::Field::Path>("/path/photo.jpeg");

    std::vector photos1{photo1};
    std::vector photos2{photo2};

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosAdded);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->addPhotos(photos1);
        transaction->abort();
        this->m_backend->addPhotos(photos2);
    }

    // transaction aborted - no notifications
    EXPECT_EQ(notifications.count(), 0);
}


TYPED_TEST(TransactionAccumulationsTests, photosUpdate)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    auto ids = this->m_backend->photoOperator().getPhotos(Database::EmptyFilter{});
    auto photo1 = this->m_backend->getPhotoDelta(ids.front());
    auto photo2 = this->m_backend->getPhotoDelta(ids.back());

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosModified);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->update({photo1});
        this->m_backend->update({photo2});
    }

    // modifications should be accumulated into one notification
    EXPECT_EQ(notifications.count(), 1);
}


TYPED_TEST(TransactionAccumulationsTests, photosUpdateAborted)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    auto ids = this->m_backend->photoOperator().getPhotos(Database::EmptyFilter{});
    auto photo1 = this->m_backend->getPhotoDelta(ids.front());
    auto photo2 = this->m_backend->getPhotoDelta(ids.back());

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosModified);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->update({photo1});
        this->m_backend->update({photo2});
        transaction->abort();
    }

    // transaction aborted - no notifications
    EXPECT_EQ(notifications.count(), 0);
}


TYPED_TEST(TransactionAccumulationsTests, photosUpdateAbortedInMiddle)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    auto ids = this->m_backend->photoOperator().getPhotos(Database::EmptyFilter{});
    auto photo1 = this->m_backend->getPhotoDelta(ids.front());
    auto photo2 = this->m_backend->getPhotoDelta(ids.back());

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosModified);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->update({photo1});
        transaction->abort();
        this->m_backend->update({photo2});
    }

    // transaction aborted - no notifications
    EXPECT_EQ(notifications.count(), 0);
}


TYPED_TEST(TransactionAccumulationsTests, groupsRemoval)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(GroupsDB::db);

    const auto groups = this->m_backend->groupOperator().listGroups();
    ASSERT_THAT(groups.size(), Eq(2));

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosModified);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->groupOperator().removeGroup(groups.front());
        this->m_backend->groupOperator().removeGroup(groups.back());
    }

    // modifications should be accumulated into one notification
    EXPECT_EQ(notifications.count(), 1);
}


TYPED_TEST(TransactionAccumulationsTests, groupsRemovalAborted)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(GroupsDB::db);

    const auto groups = this->m_backend->groupOperator().listGroups();
    ASSERT_THAT(groups.size(), Eq(2));

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosModified);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->groupOperator().removeGroup(groups.front());
        this->m_backend->groupOperator().removeGroup(groups.back());
        transaction->abort();
    }

    // transaction aborted - no notifications
    EXPECT_EQ(notifications.count(), 0);
}


TYPED_TEST(TransactionAccumulationsTests, groupsRemovalAbortedInTheMiddle)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(GroupsDB::db);

    const auto groups = this->m_backend->groupOperator().listGroups();
    ASSERT_THAT(groups.size(), Eq(2));

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosModified);
    {
        auto transaction = this->m_backend->openTransaction();
        this->m_backend->groupOperator().removeGroup(groups.front());
        transaction->abort();
        this->m_backend->groupOperator().removeGroup(groups.back());
    }

    // transaction aborted - no notifications
    EXPECT_EQ(notifications.count(), 0);
}


TYPED_TEST(TransactionAccumulationsTests, groupsCreation)
{
    Database::JsonToBackend converter(*this->m_backend);
    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosModified);

    {
        auto tr = this->m_backend->openTransaction();
        converter.append(GroupsDB::db);
    }

    // modifications should be accumulated into one notification
    EXPECT_EQ(notifications.count(), 1);
}


TYPED_TEST(TransactionAccumulationsTests, groupsCreationAborted)
{
    Database::JsonToBackend converter(*this->m_backend);
    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosModified);

    {
        auto tr = this->m_backend->openTransaction();
        converter.append(GroupsDB::db);
        tr->abort();
    }

    // transaction aborted - no notifications
    EXPECT_EQ(notifications.count(), 0);
}
