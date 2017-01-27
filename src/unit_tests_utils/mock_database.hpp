
#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <database/idatabase.hpp>
#include <database/project_info.hpp>


// depends on: https://github.com/google/googletest/issues/395
struct MockDatabase: Database::IDatabase
{
    MOCK_METHOD0(notifier, Database::ADatabaseSignals*());

    MOCK_METHOD2(exec, void(std::unique_ptr<Database::AStorePhotoTask> &&, const IPhotoInfo::Ptr &) );
    MOCK_METHOD2(store, void( const std::set<QString> &, const std::function<void(const std::vector<Photo::Id> &)> &) );
    MOCK_METHOD2(createGroup, void( const Photo::Id &, const std::function<void(Group::Id)> &) );

    MOCK_METHOD2(countPhotos, void(const std::deque<Database::IFilter::Ptr> &, const std::function<void(int)> &) );
    MOCK_METHOD2(getPhotos, void(const std::vector<Photo::Id> &, const std::function<void(std::deque<IPhotoInfo::Ptr>)> &) );
    MOCK_METHOD1(listTagNames, void(const Callback<const std::deque<TagNameInfo> &> & ) );
    MOCK_METHOD2(listTagValues, void( const TagNameInfo &, const Callback<const TagNameInfo &, const std::deque<TagValue> &> & ) );
    MOCK_METHOD3(listTagValues, void( const TagNameInfo &, const std::deque<Database::IFilter::Ptr> &, const Callback<const TagNameInfo &, const std::deque<TagValue> &> & ) );
    MOCK_METHOD2(listPhotos, void(const std::deque<Database::IFilter::Ptr> &, const Callback<const IPhotoInfo::List &> &) );

    MOCK_METHOD2(perform, void(const std::deque<Database::IFilter::Ptr> &, const std::deque<Database::IAction::Ptr> &) );

    MOCK_METHOD2(exec, void(std::unique_ptr<Database::ADropPhotosTask> &&, const std::deque<Database::IFilter::Ptr> &) );

    MOCK_METHOD2(exec, void(std::unique_ptr<Database::AInitTask> &&, const Database::ProjectInfo &) );

    MOCK_METHOD0(closeConnections, void() );
};

#endif

