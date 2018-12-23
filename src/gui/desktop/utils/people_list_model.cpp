/*
 * Model keeping list of all people names
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

#include <core/cross_thread_call.hpp>
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

        db->exec([f](Database::IBackend* op)
        {
            const std::vector<PersonName> names = op->listPeople();

            QStringList namesList;

            for (const PersonName& name: names)
                namesList.append(name.name());

            f(namesList);
        });
    }
}


QVariant PeopleListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if ( (role == Qt::EditRole || role == Qt::DisplayRole) &&
         index.column() == 0 && index.row() < rowCount(index.parent()))
    {
        result = m_names[index.row()];
    }

    return result;
}


int PeopleListModel::rowCount(const QModelIndex& parent) const
{
    const int count = parent.isValid()? 0: m_names.size();

    return count;
}


void PeopleListModel::fill(const QStringList& names)
{
    assert(m_names.empty());   // otherwise we need to call beginRemoveRows (or similar)

    const int last = names.size() - 1;

    beginInsertRows(QModelIndex(), 0, last);

    m_names = names;

    endInsertRows();
}


void PeopleListModel::clear()
{
    beginResetModel();

    m_names.clear();

    endResetModel();
}
