/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QPromise>

#include <core/function_wrappers.hpp>
#include <database/idatabase.hpp>
#include <database/igroup_operator.hpp>
#include <database/iphoto_operator.hpp>
#include <database/photo_utils.hpp>
#include <project_utils/misc.hpp>

#include "utils/grouppers/collage_generator.hpp"
#include "groups_manager.hpp"

import system;


QString GroupsManager::includeRepresentatInDatabase(const QString& representativePhoto, Project& project)
{
    const QString internalPath = includeFileInPrivateMediaLocation(project.getProjectInfo(), representativePhoto);
    const QString internalPathDecorated = project.makePathRelative(internalPath);

    return internalPathDecorated;
}


void GroupsManager::groupIntoUnified(Project& project, QPromise<void>&& promise, const std::vector<std::vector<ExplicitDelta>>& groups)
{
    std::vector<GroupDetails> groupsDetails;

    std::transform(groups.begin(), groups.end(), std::back_inserter(groupsDetails), [&project](const std::vector<ExplicitDelta>& group)
    {
        const QString representativePath = GroupsManager::includeRepresentatInDatabase(group.front().get<Photo::Field::Path>(), project);

        std::vector<Photo::Id> ids;
        std::transform(group.begin(), group.end(), std::back_inserter(ids), [](const auto& data) { return data.getId(); } );

        return GroupDetails{ .members = ids, .representativePath = representativePath, .type = Group::Type::Generic };
    });

    return group(project.getDatabase(), std::move(promise), groupsDetails);
}


void GroupsManager::group(Database::IDatabase& database,
                          const std::vector<Photo::Id>& photos,
                          const QString& representativePath,
                          Group::Type type)
{
    group(database, {}, { GroupDetails{.members = photos, .representativePath = representativePath, .type = type} });
}


void GroupsManager::group(Database::IDatabase& database, QPromise<void>&& promise, const std::vector<GroupDetails>& groups)
{
    database.exec([groups, db_promise = std::move(promise)](Database::IBackend& backend) mutable
    {
        const std::size_t groupSize = groups.size();
        assert(groupSize > 0);

        db_promise.start();
        db_promise.setProgressRange(0, groupSize - 1);
        db_promise.setProgressValue(0);

        auto transaction = backend.openTransaction();

        for (std::size_t i = 0; i < groupSize; i++)
        {
            const auto& group = groups[i];
            const auto& photos = group.members;
            if (photos.empty())
                continue;

            const QString& representativePath = group.representativePath;
            const Group::Type& type = group.type;

            // copy details of first member to representative
            const auto firstPhoto = backend.getPhotoDelta<Photo::Field::Flags, Photo::Field::Tags>(photos[0]);

            const auto firstPhotoFlags = firstPhoto.get<Photo::Field::Flags>();
            auto it = firstPhotoFlags.find(Photo::FlagsE::StagingArea);
            const Photo::FlagValues flags = { {Photo::FlagsE::StagingArea, it == firstPhotoFlags.end()? 0: it->second} };

            Photo::DataDelta data;
            data.insert<Photo::Field::Path>(representativePath);
            data.insert<Photo::Field::Tags>(firstPhoto.get<Photo::Field::Tags>());
            data.insert<Photo::Field::Flags>(flags);

            // store representative photo
            std::vector photos_to_store = {data};
            backend.addPhotos(photos_to_store);

            // create group
            const Photo::Id representativeId = photos_to_store.front().getId();
            const auto groupId = backend.groupOperator().addGroup(representativeId, type);

            // update group information on each member
            std::vector<Photo::DataDelta> deltas;
            deltas.reserve(photos.size());

            for(const auto memberId: photos)
            {
                GroupInfo grpInfo(groupId, GroupInfo::Member);

                Photo::DataDelta memberData;
                memberData.setId(memberId);
                memberData.insert<Photo::Field::GroupInfo>(grpInfo);

                deltas.push_back(memberData);
            }

            backend.update(deltas);

            db_promise.setProgressValue(i);
        }

        db_promise.finish();
    });
}


void GroupsManager::ungroup(Database::IDatabase& db, const Group::Id& gid)
{
    db.exec([gid](Database::IBackend& backend)
    {
        auto tr = backend.openTransaction();

        // dissolve group
        const Photo::Id repId = backend.groupOperator().removeGroup(gid);

        // remove representative from db
        backend.photoOperator().removePhoto(repId);
    });
}
