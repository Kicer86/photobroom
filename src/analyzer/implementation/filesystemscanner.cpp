/*
    File system scanner implementation.
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


#include "filesystemscanner.hpp"

#include <boost/filesystem.hpp>

std::vector< std::string, std::allocator< std::string > > FileSystemScanner::getFilesFor(const std::string &dir_path)
{
	std::vector<std::string> results;

	boost::filesystem::path p(dir_path);

	boost::filesystem::recursive_directory_iterator dirIt(p), dirItEnd;

	for(; dirIt != dirItEnd; ++dirIt)
	{
		const boost::filesystem::path &entry = *dirIt;
		results.push_back( entry.string() );
	}

	return results;
}

FileSystemScanner::FileSystemScanner()
{

}


FileSystemScanner::~FileSystemScanner()
{

}

