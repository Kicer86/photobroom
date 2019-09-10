/*
 * Tools for managing db transactions.
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


Transaction::Transaction(TransactionalDatabase& db): m_db(db)
{
    m_db.tr_begin();
}


Transaction::~Transaction()
{
    m_db.tr_commit();
}


TransactionalDatabase::TransactionalDatabase():
    m_connection_name(),
    m_level(0),
    m_clean(true)
{

}

void Transaction::rollback()
{
    m_db.tr_rollback();
}


void TransactionalDatabase::setConnectionName(const QString& name)
{
    m_connection_name = name;
}


void TransactionalDatabase::tr_begin()
{
    m_level++;

    if (m_level == 1)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connection_name);
        m_clean = db.transaction();

    }
}

void TransactionalDatabase::tr_commit()
{
    assert(m_level > 0);
    m_level--;

    if (m_level == 0)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connection_name);

        if (m_clean)
            db.commit();
        else
            db.rollback();
    }
}


void TransactionalDatabase::tr_rollback()
{
    m_clean = false;
    tr_commit();
}

