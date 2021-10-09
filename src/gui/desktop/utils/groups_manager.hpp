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

#ifndef GROUPS_MANAGER_HPP
#define GROUPS_MANAGER_HPP

#include <core/iexif_reader.hpp>
#include <database/photo_data.hpp>
#include <database/group.hpp>
#include <project_utils/project.hpp>


namespace Database
{
    struct IDatabase;
}


namespace GroupsManager
{
    struct GroupDetails
    {
        std::vector<Photo::Id> members;
        QString representativePath;
        Group::Type type;
    };

    QString includeRepresentatInDatabase(const QString& representativePhoto, Project &);

    void groupIntoCollage(IExifReaderFactory &,
                          Project &,
                          const std::vector<Photo::Data> &);

    void groupIntoUnified(Project &,
                          const std::vector<Photo::Data> &);

    void groupIntoUnified(Project &,
                          QPromise<void> &&,
                          const std::vector<std::vector<Photo::Data>> &);       // create many groups at once

    void group(Database::IDatabase &,
               const std::vector<Photo::Data> &,
               const QString& representativePath,
               Group::Type);                                 // group set of photos as one with given (external/generated) representative

    void group(Database::IDatabase &,
               const std::vector<Photo::Id> &,
               const QString& representativePath,
               Group::Type);                                 // group set of photos as one with given (external/generated) representative

    void group(Database::IDatabase &, QPromise<void> &&, const std::vector<GroupDetails> &);

    void ungroup(Database::IDatabase &, const Group::Id &);
}

#endif // GROUPS_MANAGER_HPP
