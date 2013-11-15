/*
    Helper class.
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


#ifndef IMAGE_MANAGER_HPP
#define IMAGE_MANAGER_HPP

#include <QPixmap>

class QAbstractItemModel;

struct ImageManager
{
    public:
        ImageManager(QAbstractItemModel *);
        ImageManager(const ImageManager &) = delete;
        ~ImageManager() ;

        void operator=(const ImageManager &) = delete;    
        QSize size(int i) const;           //size of 'index' item in model    
        void draw(int i, QPainter *painter, const QRect &rect) const;    
        QPixmap getPixmap(int i) const;

    private:
        QAbstractItemModel *m_model;
};    

#endif // IMAGE_MANAGER_HPP
