/*
    set of filesystem access functions
    Copyright (C) 2013  Micha³ Walenciak <MichalWalenciak@gmail.com>

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


#ifndef IFS_HPP
#define IFS_HPP

#include <iostream>

struct FS
{
    virtual ~FS() {}
        
    //open and return file
	virtual std::iostream* openStream(const std::string &filename, 
                                      std::ios_base::openmode mode = std::ios_base::in|std::ios_base::out ) = 0;

    //close opened file
    virtual void closeStream(std::iostream *) = 0;
};

#endif
