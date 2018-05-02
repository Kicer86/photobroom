/*
 *    interface for backends
 *    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef IBACKEND_HPP
#define IBACKEND_HPP

#include <string>
#include <set>
#include <vector>

#include <core/tag.hpp>

#include "action.hpp"
#include "database_status.hpp"
#include "filter.hpp"
#include "group.hpp"
#include "person_data.hpp"
#include "photo_data.hpp"

#include "database_export.h"

struct ILoggerFactory;
struct IConfiguration;

namespace Database
{
    struct ProjectInfo;

    //Low level database interface.
    //To be used by particular database backend
    struct DATABASE_EXPORT IBackend
    {
        virtual ~IBackend() = default;

        //add photo to database
        virtual bool addPhotos(std::vector<Photo::DataDelta> &) = 0;

        // create group
        virtual Group::Id addGroup(const Photo::Id &) = 0;

        //update data
        virtual bool update(const Photo::DataDelta &) = 0;

        //read data
        virtual std::vector<TagNameInfo> listTags() = 0;                                         // list all stored tag names
        virtual std::vector<TagValue>    listTagValues(const TagNameInfo &,
                                                       const std::vector<IFilter::Ptr> &) = 0;   // list all values of tag for photos matching provided filter
        virtual std::vector<Photo::Id>   getAllPhotos() = 0;                                     // list all photos
        virtual std::vector<Photo::Id>   getPhotos(const std::vector<IFilter::Ptr> &) = 0;       // find all photos matching filter
        virtual std::vector<Photo::Id>   dropPhotos(const std::vector<IFilter::Ptr> &) = 0;      // drop photos matching filter
        virtual Photo::Data              getPhoto(const Photo::Id &) = 0;                        // get particular photo
        virtual int                      getPhotosCount(const std::vector<IFilter::Ptr> &) = 0;  // is there any photo matching filters?
        virtual QList<QVariant>          find(const QString &) = 0;                              // find items matching query
        virtual std::vector<PersonData>  listPeople() = 0;                                       // list all people
        virtual std::vector<PersonLocation> listPeople (const Photo::Id &) = 0;                  // list people on photo
        virtual PersonData               person(const Person::Id &) = 0;                         // person data
        virtual std::vector<FaceData>    listFaces(const Photo::Id &) = 0;                       // list faces on photo
        virtual Person::Id               store(const PersonData &) = 0;                          // store or update person data
        virtual Face::Id                 store(const FaceData &) = 0;                            // store or update face data
        virtual void                     store(const Person::Id &,
                                               const Face::Id &) = 0;                            // store information about person on photo

        // reading extra data
        //virtual QByteArray getThumbnail(const Photo::Id &) = 0;                               // get thumbnail for photo

        // modify data
        virtual void perform(const std::vector<Database::IFilter::Ptr> &, const std::vector<Database::IAction::Ptr> &) = 0;

        // write extra data
        //virtual bool setThumbnail(const Photo::Id &, const QByteArray &) = 0;                  // set thumbnail for photo

        //init backend - connect to database or create new one
        virtual BackendStatus init(const ProjectInfo &) = 0;

        //configuration
        virtual void set(IConfiguration *) = 0;
        virtual void set(ILoggerFactory *) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };
}

#endif

