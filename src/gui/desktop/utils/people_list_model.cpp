/*
 * Photo Broom - photos management tool.
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

#include "people_list_model.hpp"

#include <core/function_wrappers.hpp>
#include <database/idatabase.hpp>

#include <functional>


using namespace std::placeholders;


PeopleListModel::PeopleListModel()
{

}


PeopleListModel::~PeopleListModel()
{

}


void PeopleListModel::setDB(Database::IDatabase* db)
{
    if (db == nullptr)
        clear();
    else
    {
        // TODO: here we fetch all existing people.
        // PeopleListModel should react on changes in db,
        // but there is no proper signal from IDatabase.
        //
        // To consider: as functionality of IDatabase is being limited and moved
        // to IBackend, maybe all signals should be moved there also.

        auto f = make_cross_thread_function<const QStringList &>(this, std::bind(&PeopleListModel::fill, this, _1));

        db->exec([f](Database::IBackend& op)
        {
            const std::vector<PersonName> names = op.peopleInformationAccessor().listPeople();

            QStringList namesList;

            for (const PersonName& name: names)
                namesList.append(name.name());

            f(namesList);
        },
        "PeopleListModel: collect names"
        );
    }
}


const QStringList& PeopleListModel::data() const
{
    return m_names;
}


void PeopleListModel::fill(const QStringList& names)
{
    assert(m_names.empty());   // otherwise we need to call beginRemoveRows (or similar)

    if (names.empty() == false)
    {
        m_names = names;

        emit dataChanged();
    }
}


void PeopleListModel::clear()
{
    m_names.clear();

    emit dataChanged();
}
