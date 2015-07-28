/*
 * Flat list model with lazy image load.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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
 *
 */

#ifndef LISTMODELPRIVATE_HPP
#define LISTMODELPRIVATE_HPP


#include <deque>

#include <QPixmap>


struct Info
{
    QString path;
    QPixmap pixmap;
    QString filename;

    Info(const QString& p): path(p) {}
};


class ListModelPrivate
{
    public:
        ListModelPrivate(ListModel* q);
        virtual ~ListModelPrivate();

        std::deque<Info> m_data;

    private:
        class ListModel* const q;
};

#endif // LISTMODELPRIVATE_HPP
