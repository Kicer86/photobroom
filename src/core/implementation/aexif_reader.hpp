/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef A_EXIF_READER_HPP
#define A_EXIF_READER_HPP

#include <thread>

#include "iexif_reader.hpp"


class QByteArray;


class AExifReader: public IExifReader
{
    public:
        AExifReader();
        AExifReader (const AExifReader &) = delete;
        virtual ~AExifReader();

        AExifReader& operator=(const AExifReader &) = delete;

    protected:
        virtual void collect(const QString &) = 0;
        virtual std::optional<std::string> read(TagType) const = 0;

    private:
        std::thread::id m_id;

        // ITagFeeder:
        Tag::TagsList getTagsFor(const QString& path) override;
        std::optional<std::any> get(const QString& path, const TagType &) override;
        //

        Tag::TagsList feedDateAndTime() const;

        // reading various tag types
        std::optional<int> readInt(const TagType &) const;
        std::optional<std::string> readString(const TagType &) const;
        std::optional<long> readLong(const TagType &) const;
        std::optional<float> readRational(const TagType &) const;
};

#endif
