
/*
 * Widget for photo properties
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef PHOTOPROPERTIES_HPP
#define PHOTOPROPERTIES_HPP

#include <QWidget>

#include <database/iphoto_info.hpp>

class QLabel;
class QStackedLayout;

class SelectionExtractor;
struct APhotoInfoModel;


class PhotoProperties: public QWidget
{
        Q_OBJECT

    public:
        PhotoProperties(QWidget * = nullptr);
        PhotoProperties(const PhotoProperties &) = delete;
        ~PhotoProperties();

        PhotoProperties& operator=(const PhotoProperties &) = delete;

        void set(const SelectionExtractor *);

    private:
        const SelectionExtractor* m_selectionExtractor;
        QLabel* m_locationLabel;
        QLabel* m_sizeLabel;
        QLabel* m_locationValue;
        QLabel* m_sizeValue;

        void refreshView() const;
        void refreshLabels(const std::vector<IPhotoInfo::Ptr> &) const;
        void refreshValues(const std::vector<IPhotoInfo::Ptr> &) const;

        QString sizeHuman(int) const;
};

#endif // PHOTOPROPERTIES_HPP
