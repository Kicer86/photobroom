/*
    Helper class for Images View.
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef POSITIONS_CACHE_HPP
#define POSITIONS_CACHE_HPP

#include <memory>

#include <QRect>

class QAbstractItemView;

class PositionsCache
{
    public:
        PositionsCache(QAbstractItemView* view);
        ~PositionsCache();

        void invalidate() const;
        size_t items() const;
        QRect pos(int i) const;

    private:
        struct MutableData;

        std::unique_ptr<MutableData> m_data;

        void flushData() const;
        void validateCache() const;
        void reloadCache() const;
        void updateScrollBars() const;
};

#endif // POSITIONS_CACHE_HPP
