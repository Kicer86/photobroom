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
#include <optional>

#include <core/tag.hpp>

#include "action.hpp"
#include "database_status.hpp"
#include "filter.hpp"
#include "group.hpp"
#include "person_data.hpp"
#include "photo_data.hpp"

#include "database_export.h"


struct IConfiguration;
struct ILoggerFactory;


#define DB_ERR_ON_FALSE(CALL)   \
    if ( !(CALL) )              \
        throw db_error {}


namespace Database
{
    struct IGroupOperator;
    struct ProjectInfo;

    // for internal usage
    class db_error: std::exception {};

    //Low level database interface.
    //To be used by particular database backend
    // TODO: divide into smaller interfaces and use repository pattern (see github issue #272)
    //       Backend should be splitted into operator and itself should become a facade.
    struct DATABASE_EXPORT IBackend: public QObject
    {
        virtual ~IBackend() = default;

        //add photo to database
        virtual bool addPhotos(std::vector<Photo::DataDelta> &) = 0;

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
        virtual std::vector<PersonName>  listPeople() = 0;                                       // list all people names
        virtual std::vector<PersonInfo>  listPeople(const Photo::Id &) = 0;                      // list people on photo
        virtual PersonName               person(const Person::Id &) = 0;                         // person data
        virtual Person::Id               store(const PersonName &) = 0;                          // store or update person
        virtual PersonInfo::Id           store(const PersonInfo &) = 0;                          // store or update person details
        virtual void                     set(const Photo::Id &, const QString &, int value) = 0; // set flag for photo to given value
        virtual std::optional<int>       get(const Photo::Id &, const QString &) = 0;            // get flag value

        // reading extra data
        //virtual QByteArray getThumbnail(const Photo::Id &) = 0;                               // get thumbnail for photo

        // modify data
        virtual void perform(const std::vector<Database::IFilter::Ptr> &, const std::vector<Database::IAction::Ptr> &) = 0;
        virtual std::vector<Photo::Id> markStagedAsReviewed() = 0;

        // write extra data
        //virtual bool setThumbnail(const Photo::Id &, const QByteArray &) = 0;                  // set thumbnail for photo

        //init backend - connect to database or create new one
        virtual BackendStatus init(const ProjectInfo &) = 0;

        //close database connection
        virtual void closeConnections() = 0;

        // TODO: a set of 'operators' which are about to replace methods above
        //       in the name of interface segregation and repository pattern (see #272 on github)
        virtual IGroupOperator* groupOperator() = 0;

    signals:
        void photosAdded(const std::vector<Photo::Id> &);               // emited after new photos were added to database
        void photoModified(const Photo::Id &);                          // emited when photo updated
        void photosRemoved(const std::vector<Photo::Id> &);             // emited after photos removal
        void photosMarkedAsReviewed(const std::vector<Photo::Id> &);    // emited when done with photos marking

    private:
        Q_OBJECT
    };
}

#endif

