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

#include "transaction.hpp"
#include <QSqlDatabase>


Transaction::Transaction(NestedTransaction& db):
    m_db(db),
    m_commited(false)
{

}


Transaction::~Transaction()
{
    if (m_commited == false)
        m_db.tr_rollback();
}


bool Transaction::begin()
{
    return m_db.tr_begin();
}


bool Transaction::commit()
{
    m_commited = true;
    return m_db.tr_commit();
}


void Transaction::rollback()
{
    m_db.tr_rollback();
}


NestedTransaction::NestedTransaction():
    m_connection_name(),
    m_level(0),
    m_clean(true)
{

}


void NestedTransaction::setConnectionName(const QString& name)
{
    m_connection_name = name;
}


bool NestedTransaction::tr_begin()
{
    bool status = true;
    m_level++;

    if (m_level == 1)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connection_name);
        status = db.transaction();
        m_clean = status;
    }

    return status;
}

bool NestedTransaction::tr_commit()
{
    assert(m_level > 0);

    bool status = false;
    m_level--;

    if (m_level == 0)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connection_name);

        if (m_clean)
        {
            status = db.commit();

            emit commited();
        }
        else
        {
            db.rollback();

            emit rolledback();
        }
    }
    else
        status = true;

    return status;
}


bool NestedTransaction::tr_rollback()
{
    m_clean = false;
    return tr_commit();
}

