/*
 * helper for finding best face
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef FACEOPTIMIZER_HPP
#define FACEOPTIMIZER_HPP

#include <QObject>

#include <core/callback_ptr.hpp>
#include <database/person_data.hpp>
#include <system/system.hpp>

#include "face_details.hpp"

namespace Database
{
    struct IDatabase;
}

struct ICoreFactoryAccessor;
class PeopleOperator;

class FaceOptimizer: public IModelFaceFinder
{
    public:
        FaceOptimizer(Database::IDatabase *,
                      ICoreFactoryAccessor *,
                      PeopleOperator *);

        ~ FaceOptimizer();

        void set(const std::map<Photo::Id, QString> &);

        void findBest(const std::vector<PersonInfo> &,
                      const std::function<void(const QString &)> &) override;

        QString current(const Person::Id &) const override;

    private:
        PeopleOperator* m_operator;
        safe_callback_ctrl m_safe_callback;
        std::unique_ptr<ITmpDir> m_tmpDir;
        std::map<Photo::Id, QString> m_photo2path;
        Database::IDatabase* m_db;
        ICoreFactoryAccessor* m_core;

        std::map<QString, PersonInfo> saveFiles(const std::vector<PersonInfo> &);
};

#endif // FACEOPTIMIZER_HPP
