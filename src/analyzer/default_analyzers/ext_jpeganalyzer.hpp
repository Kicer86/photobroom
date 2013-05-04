/*
    <one line to give the program's name and a brief idea of what it does.>
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


#ifndef EXT_JPEGANALYZER_HPP
#define EXT_JPEGANALYZER_HPP

#include <../../home/michal/projects/sources/broom/src/analyzer/ianalyzer.hpp>


class Ext_JpegAnalyzer : public IAnalyzer
{

    public:
        virtual bool isImage(const std::string& );
        Ext_JpegAnalyzer();
        Ext_JpegAnalyzer(const Ext_JpegAnalyzer& other);
        virtual ~Ext_JpegAnalyzer();
        virtual Ext_JpegAnalyzer& operator=(const Ext_JpegAnalyzer& other);
        virtual bool operator==(const Ext_JpegAnalyzer& other) const;
};

#endif // EXT_JPEGANALYZER_HPP
