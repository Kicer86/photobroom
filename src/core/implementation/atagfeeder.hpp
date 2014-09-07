
/*
* Base for tag feeders
* Copyright (C) 2014  Micha³ Walenciak <MichalWalenciak@gmail.com>
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

#ifndef A_TAG_FEEDER_HPP
#define A_TAG_FEEDER_HPP

#include "itagfeeder.hpp"

class QByteArray;
struct ITagData;

class ATagFeeder: public ITagFeeder
{
    public:
        ATagFeeder();
        virtual ~ATagFeeder();

    protected:
        enum TagTypes
        {
            DateTimeOriginal,
        };

        virtual void collect(const QByteArray &) = 0;
        virtual std::string get(TagTypes) = 0;

    private:
        // ITagFeeder:
        Tag::TagsList getTagsFor(const QString& path) override;
        //void update(Tag::TagsList *, const QString& path) override;
        //

        void feedDateAndTime(Tag::TagsList &);
};

#endif // A_TAG_FEEDER_HPP
