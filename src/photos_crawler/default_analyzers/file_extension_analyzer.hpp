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


#ifndef EXT_DEFAULTANALYZER_HPP
#define EXT_DEFAULTANALYZER_HPP

#include <QRegExp>

#include "ianalyzer.hpp"


class FileExtensionAnalyzer : public IAnalyzer
{
    public:
        FileExtensionAnalyzer();
        virtual ~FileExtensionAnalyzer();

        virtual bool isImage(const QString &) override;

    private:
        const QRegExp m_ext_regex;

        FileExtensionAnalyzer(const FileExtensionAnalyzer &) = delete;
        virtual FileExtensionAnalyzer& operator=(const FileExtensionAnalyzer &) = delete;
        virtual bool operator==(const FileExtensionAnalyzer &) const = delete;
};

#endif // EXT_JPEGANALYZER_HPP
