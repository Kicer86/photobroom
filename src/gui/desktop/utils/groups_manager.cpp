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

#include "groups_manager.hpp"

#include <core/function_wrappers.hpp>
#include <database/idatabase.hpp>
#include <database/igroup_operator.hpp>
#include <database/iphoto_operator.hpp>


void GroupsManager::group(Database::IDatabase* database,
                          const std::vector<Photo::Id>& photos,
                          const QString& representativePath,
                          Group::Type type)
{
    if (photos.empty() == false)
    {
        database->exec([photos, representativePath, type](Database::IBackend& backend)
        {
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
        });
    }
}


void GroupsManager::ungroup(Database::IDatabase* db, const Group::Id& gid)
{
    db->exec([gid](Database::IBackend& backend)
    {
        // dissolve group
        const Photo::Id repId = backend.groupOperator().removeGroup(gid);

        // remove representative from db
        backend.photoOperator().removePhoto(repId);
    });
}
