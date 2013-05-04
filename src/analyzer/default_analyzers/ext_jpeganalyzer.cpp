/*
    Extension base analyzer for JPEG files
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


#include "ext_jpeganalyzer.hpp"

#include <regex>

#include <boost/filesystem.hpp>

Ext_JpegAnalyzer::Ext_JpegAnalyzer()
{

}

Ext_JpegAnalyzer::Ext_JpegAnalyzer(const Ext_JpegAnalyzer &)
{

}

Ext_JpegAnalyzer::~Ext_JpegAnalyzer()
{

}


bool Ext_JpegAnalyzer::isImage(const std::string &file_path)
{
    boost::filesystem::path path(file_path);
    
    const std::string ext = path.extension().string();
    
    std::regex ext_regex("[Jj][Pp][Ee]?[Gg]");
    const bool matches = std::regex_match(ext, ext_regex);
    
    return matches;
}


Ext_JpegAnalyzer& Ext_JpegAnalyzer::operator=(const Ext_JpegAnalyzer &)
{
    return *this;
}

bool Ext_JpegAnalyzer::operator==(const Ext_JpegAnalyzer &) const
{
    return true;
}

