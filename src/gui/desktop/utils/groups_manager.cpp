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
#include <system/system.hpp>

#include "utils/grouppers/collage_generator.hpp"
#include "groups_manager.hpp"


QString GroupsManager::includeRepresentatInDatabase(const QString& representativePhoto, Project& project)
{
    const QString internalPath = includeFileInPrivateMediaLocation(project.getProjectInfo(), representativePhoto);
    const QString internalPathDecorated = project.makePathRelative(internalPath);

    return internalPathDecorated;
}


void GroupsManager::groupIntoCollage(
    IExifReaderFactory& exifFactory,
    Project& project,
    const std::vector<Photo::Data>& photos)
{
    QStringList paths;
    std::transform(photos.begin(), photos.end(), std::back_inserter(paths), [](const auto& data) { return data.path; });

    const Photo::Data highest = *std::max_element(photos.begin(), photos.end(), [](const auto& lhs, const auto& rhs)
    {
        return lhs.geometry.height() < rhs.geometry.height();
    });

    CollageGenerator generator(exifFactory.get());
    const auto collage = generator.generateCollage(paths, highest.geometry.height());

    auto tmpDir = System::createTmpDir("CollageGenerator", System::BigFiles | System::Confidential);
    const QString collagePath = System::getUniqueFileName(tmpDir->path(), "jpeg");
    collage.save(collagePath);

    const QString representantPath = GroupsManager::includeRepresentatInDatabase(collagePath, project);
    GroupsManager::group(project.getDatabase(), photos, representantPath, Group::Type::Generic);
}


void GroupsManager::groupIntoUnified(
    Project& project,
    const std::vector<Photo::Data>& photos)
{
    const QString representantPath = GroupsManager::includeRepresentatInDatabase(photos.front().path, project);
    GroupsManager::group(project.getDatabase(), photos, representantPath, Group::Type::Generic);
}


void GroupsManager::groupIntoUnified(Project& project, QPromise<void>&& promise, const std::vector<std::vector<Photo::Data>>& groups)
{
    std::vector<GroupDetails> groupsDetails;

    std::transform(groups.begin(), groups.end(), std::back_inserter(groupsDetails), [&project](const auto& group)
    {
        const QString representativePath = GroupsManager::includeRepresentatInDatabase(group.front().path, project);

        std::vector<Photo::Id> ids;
        std::transform(group.begin(), group.end(), std::back_inserter(ids), Photo::getId);

        return GroupDetails{ .members = ids, .representativePath = representativePath, .type = Group::Type::Generic };
    });

    return group(project.getDatabase(), std::move(promise), groupsDetails);
}


void GroupsManager::group(Database::IDatabase& database,
                          const std::vector<Photo::Data>& photos,
                          const QString& representativePath,
                          Group::Type type)
{
    std::vector<Photo::Id> photos_ids;
    std::transform(photos.begin(), photos.end(), std::back_inserter(photos_ids), [](const auto& data){ return data.id; });

    group(database, photos_ids, representativePath, type);
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
            const Photo::Data firstPhoto = backend.getPhoto(photos[0]);

            auto it = firstPhoto.flags.find(Photo::FlagsE::StagingArea);
            const Photo::FlagValues flags = { {Photo::FlagsE::StagingArea, it == firstPhoto.flags.end()? 0: it->second} };

            Photo::DataDelta data;
            data.insert<Photo::Field::Path>(representativePath);
            data.insert<Photo::Field::Tags>(firstPhoto.tags);
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
        // dissolve group
        const Photo::Id repId = backend.groupOperator().removeGroup(gid);

        // remove representative from db
        backend.photoOperator().removePhoto(repId);
    });
}
