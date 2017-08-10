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


#include "file_extension_analyzer.hpp"

//#include <regex>  doesn't work as expected in gcc 4.7.2

#include <QFileInfo>

FileExtensionAnalyzer::FileExtensionAnalyzer(): m_ext_regex("jpe?g", Qt::CaseInsensitive)
{

}


FileExtensionAnalyzer::~FileExtensionAnalyzer()
{

}


bool FileExtensionAnalyzer::isImage(const QString &file_path)
{
    QFileInfo path(file_path);
    const QString ext = path.suffix();
    const bool matches = m_ext_regex.exactMatch(ext);

    return matches;
}

