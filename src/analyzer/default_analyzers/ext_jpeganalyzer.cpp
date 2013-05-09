/*
    Extension base analyzer for JPEG, PNG, BMP, TIFF and some other files
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

//#include <regex>  doesn't work as expected in gcc 4.7.2

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace
{
    const boost::regex ext_regex("\\.jpe?g", boost::regex_constants::ECMAScript | 
                                             boost::regex::icase | 
                                             boost::regex::optimize);
}


Ext_DefaultAnalyzer::Ext_DefaultAnalyzer()
{

}


Ext_DefaultAnalyzer::Ext_DefaultAnalyzer(const Ext_DefaultAnalyzer &)
{

}


Ext_DefaultAnalyzer::~Ext_DefaultAnalyzer()
{

}


bool Ext_DefaultAnalyzer::isImage(const std::string &file_path)
{
    boost::filesystem::path path(file_path);    
    const std::string ext = path.extension().string();    
    const bool matches = boost::regex_match(ext, ext_regex);
    
    return matches;
}


Ext_DefaultAnalyzer& Ext_DefaultAnalyzer::operator=(const Ext_DefaultAnalyzer &)
{
    return *this;
}


bool Ext_DefaultAnalyzer::operator==(const Ext_DefaultAnalyzer &) const
{
    return true;
}

