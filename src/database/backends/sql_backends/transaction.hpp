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

#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <QObject>

class NestedTransaction;


class Transaction final
{
    public:
        explicit Transaction(NestedTransaction &);
        ~Transaction();

        bool begin();
        bool commit();
        void rollback();

    private:
        NestedTransaction& m_db;
        bool m_commited;
};


class NestedTransaction final: public QObject
{
        Q_OBJECT

    public:
        NestedTransaction();

        void setConnectionName(const QString &);

    signals:
        void commited();
        void rolledback();

    private:
        friend class Transaction;

        QString m_connection_name;
        int m_level;
        bool m_clean;

        bool tr_begin();
        bool tr_commit();
        bool tr_rollback();
};

#endif
