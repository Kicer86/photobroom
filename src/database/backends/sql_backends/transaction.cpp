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

#include <QSqlDatabase>

#include "transaction.hpp"


SqlTransaction::SqlTransaction(const QString& connectionName, Database::NotificationsAccumulator& notifications)
    : m_connection_name(connectionName)
    , m_notifications(notifications)
{
    QSqlDatabase db = QSqlDatabase::database(m_connection_name);
    db.transaction();
}


void SqlTransaction::commit()
{
    QSqlDatabase db = QSqlDatabase::database(m_connection_name);
    db.commit();

    m_notifications.fireChanges();
}


void SqlTransaction::rollback()
{
    QSqlDatabase db = QSqlDatabase::database(m_connection_name);
    db.rollback();

    m_notifications.ignoreChanges();
}
