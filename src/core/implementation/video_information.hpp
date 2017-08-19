/*
 * Toolkit for reading video information
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

#ifndef VIDEOINFORMATION_HPP
#define VIDEOINFORMATION_HPP

#include "imedia_information.hpp"

class VideoInformation : public IMediaInformation
{
    public:
        VideoInformation();
        VideoInformation(const VideoInformation &) = delete;
        VideoInformation(VideoInformation &&) = delete;

        VideoInformation& operator=(const VideoInformation &) = delete;
        VideoInformation& operator=(VideoInformation &&) = delete;

        virtual ~VideoInformation() = default;

        virtual QSize size(const QString &) const;
};

#endif // VIDEOINFORMATION_HPP
