
#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <database/idatabase.hpp>
#include <database/project_info.hpp>


// depends on: https://github.com/google/googletest/issues/395
struct MockDatabase: Database::IDatabase
{
    MOCK_METHOD1(update, void(const IPhotoInfo::Ptr &) );
    MOCK_METHOD1(update, void(const Photo::DataDelta &) );
    MOCK_METHOD1(store,  void(const std::vector<Photo::DataDelta> &) );
    MOCK_METHOD3(createGroup, void( const Photo::Id &, Group::Type, const std::function<void(Group::Id)> &) );

    MOCK_METHOD2(countPhotos, void(const std::vector<Database::IFilter::Ptr> &, const std::function<void(int)> &) );
    MOCK_METHOD2(getPhotos, void(const std::vector<Photo::Id> &, const std::function<void(const std::vector<IPhotoInfo::Ptr> &)> &) );
    MOCK_METHOD1(listTagNames, void(const Callback<const std::vector<TagTypeInfo> &> & ) );
    MOCK_METHOD2(listTagValues, void( const TagTypeInfo &, const Callback<const TagTypeInfo &, const std::vector<TagValue> &> & ) );
    MOCK_METHOD3(listTagValues, void( const TagTypeInfo &, const std::vector<Database::IFilter::Ptr> &, const Callback<const TagTypeInfo &, const std::vector<TagValue> &> & ) );
    MOCK_METHOD2(listPhotos, void(const std::vector<Database::IFilter::Ptr> &, const Callback<const IPhotoInfo::List &> &) );

    MOCK_METHOD0(markStagedAsReviewed, void());

    MOCK_METHOD0(utils,   Database::IUtils*());
    MOCK_METHOD0(backend, Database::IBackend*());

    MOCK_METHOD(void, execute, (std::unique_ptr<Database::IDatabase::ITask> &&), (override));

    MOCK_METHOD2(init, void(const Database::ProjectInfo &, const Callback<const Database::BackendStatus &> &) );

    MOCK_METHOD0(closeConnections, void() );
};

#endif

