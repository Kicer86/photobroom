
#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <database/idatabase.hpp>
#include <database/project_info.hpp>


// depends on: https://github.com/google/googletest/issues/395
struct MockDatabase: Database::IDatabase
{
    MOCK_METHOD0(notifier, Database::ADatabaseSignals*());

    MOCK_METHOD1(update, void(const IPhotoInfo::Ptr &) );
    MOCK_METHOD2(store, void( const std::vector<Photo::Data> &, const std::function<void(const std::vector<Photo::Id> &)> &) );
    MOCK_METHOD2(createGroup, void( const Photo::Id &, const std::function<void(Group::Id)> &) );

    MOCK_METHOD2(countPhotos, void(const std::vector<Database::IFilter::Ptr> &, const std::function<void(int)> &) );
    MOCK_METHOD2(getPhotos, void(const std::vector<Photo::Id> &, const std::function<void(const std::vector<IPhotoInfo::Ptr> &)> &) );
    MOCK_METHOD1(listTagNames, void(const Callback<const std::vector<TagNameInfo> &> & ) );
    MOCK_METHOD2(listTagValues, void( const TagNameInfo &, const Callback<const TagNameInfo &, const std::vector<TagValue> &> & ) );
    MOCK_METHOD3(listTagValues, void( const TagNameInfo &, const std::vector<Database::IFilter::Ptr> &, const Callback<const TagNameInfo &, const std::vector<TagValue> &> & ) );
    MOCK_METHOD2(listPhotos, void(const std::vector<Database::IFilter::Ptr> &, const Callback<const IPhotoInfo::List &> &) );

    MOCK_METHOD1(performCustomAction, void(const std::function<void(Database::IBackendOperator *)> &) );

    MOCK_METHOD2(init, void(const Database::ProjectInfo &, const Callback<const Database::BackendStatus &> &) );

    MOCK_METHOD0(closeConnections, void() );
};

#endif

