/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef IBACKEND_HPP
#define IBACKEND_HPP

#include <set>
#include <string>
#include <vector>
#include <optional>
#include <source_location>
#include <magic_enum.hpp>

#include <core/tag.hpp>

#include "database_status.hpp"
#include "filter.hpp"
#include "group.hpp"
#include "person_data.hpp"
#include "explicit_photo_delta.hpp"
#include "ipeople_information_accessor.hpp"
#include "itransaction.hpp"
#include "database_export.h"


struct IConfiguration;
struct ILoggerFactory;

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

namespace Database
{
    struct IGroupOperator;
    struct IPhotoChangeLogOperator;
    struct IPhotoOperator;
    struct ProjectInfo;

    // for internal usage
    class db_error final: std::exception
    {
            std::string m_err;
            StatusCodes m_status;

        public:
            db_error(const std::string& err, StatusCodes status = StatusCodes::GeneralError, const std::string& details = std::string()):
                m_err(),
                m_status(status)
            {
                m_err += err + ": ";
                m_err.append(magic_enum::enum_name(m_status));

                if (details.empty() == false)
                    m_err += ", " + details;
            }

            StatusCodes status() const noexcept
            {
                return m_status;
            }

            const char* what() const noexcept override
            {
                return m_err.c_str();
            }
    };

    /** \brief Low level database interface.
     *
     * It defines way of communication with database backend.\n
     * \todo Divide into smaller interfaces and use repository pattern (see github issue #272)\n
     *       Backend should be splitted into operator and itself should become a facade.
     */
    struct DATABASE_EXPORT IBackend: public QObject
    {
        virtual ~IBackend() = default;

        /** \brief Add photos to database
         *  @param photos details of photos to be stored. At least id and path need to be valid. \see Photo::DataDelta
         *  \return true if successful.
         *
         *  If operation is sucessful photosAdded() signal is emited.
         */
        virtual bool addPhotos(std::vector<Photo::DataDelta>& photos) = 0;

        /**
         * \brief update photos details
         * @param delta set of photos details to be updated
         *
         * Method updates (modifies or removes) photos details \n
         * provided in \a delta                                \n
         * If particular Photo::Field is not stored in delta   \n
         * it won't be modified.
         * \see Photo::Field
         */
        virtual bool update(const std::vector<Photo::DataDelta>& delta) = 0;

        /// list all values of tag for photos matching provided filter
        virtual std::vector<TagValue>    listTagValues(const Tag::Types &,
                                                       const Filter &) = 0;

        /**
         * @brief fetch photo data
         *
         * @param id id of photo to fetch
         * @param fields list of photo details to read
         *
         * @return structure containing chosen fields
         */
        virtual Photo::DataDelta  getPhotoDelta(const Photo::Id& id, const std::set<Photo::Field>& fields = Photo::AllFields) = 0;

        /**
         * @brief fetch photo data
         *
         * @param id id of photo to fetch
         * @tparam fields list of photo details to read
         *
         * @return explicit structure containing chosen fields
         *
         */
        template<Photo::Field... fields>
        Photo::ExplicitDelta<fields...>    getPhotoDelta(const Photo::Id& id)
        {
            std::set<Photo::Field> f;

            auto append = [&f](Photo::Field field) { f.insert(field); };
            (..., append(fields));

            return Photo::ExplicitDelta<fields...>(getPhotoDelta(id, f));
        }

        /// Count photos matching filter
        virtual int                      getPhotosCount(const Filter &) = 0;

        /**
         * \brief set flag for photo to given value
         * @param id id of photo
         * @param name flag name
         * @param value flag value to set
         *
         * Method sets flag with given value on photo with given id.
         */
        virtual void                     set(const Photo::Id& id, const QString& name, int value) = 0;

        template<typename E>
        requires std::is_enum_v<E>
        inline void set(const Photo::Id& id, const QString& name, E value)
        {
            set(id, name, static_cast<int>(value));
        }

        /**
         * \brief get flag value
         * @param id id of photo
         * @param name flag name
         * \return flag value
         *
         * Method reads flag value for given photo.
         */
        virtual std::optional<int>       get(const Photo::Id& id, const QString& name) = 0;

        /**
         * @brief set bits for provided flag
         * @param id id of photo
         * @param name flag name
         * @param bits bits to be set
         *
         * This method is similar to @ref set and @ref clearBits but allow to set particular bits of stored value
         */
        virtual void setBits(const Photo::Id& id, const QString& name, int bits) = 0;

        /**
         * @brief clear bits for provided flag
         * @param id id of photo
         * @param name flag name
         * @param bits bits to be cleared (zeroed)
         *
         * This method is similar to @ref set and @ref setBits but allow to clear particular bits of stored value
         */
        virtual void clearBits(const Photo::Id& id, const QString& name, int bits) = 0;

        /**
         * @brief Write @p blob of type @p bt
         * @param id associated photo id
         * @param bt blob type
         * @param blob raw data
         */
        virtual void writeBlob(const Photo::Id& id, const QString& bt, const QByteArray& blob) = 0;

        /**
         * @brief Read blob of type @p bt associated with photo @p id
         * @param id photo id
         * @param bt blob type
         * @return raw data
         */
        virtual QByteArray readBlob(const Photo::Id& id, const QString& bt) = 0;

        /**
         * \brief mark all staged photos as reviewed.
         * \return list of touched photos
         *
         * Method for massive marking photos as reviewed.   \n
         * When done, photosMarkedAsReviewed() signal is emited.
         */
        virtual std::vector<Photo::Id> markStagedAsReviewed() = 0;

        /// \brief init backend - connect to database or create new one
        virtual BackendStatus init(const ProjectInfo &) = 0;

        /// \brief close database connection
        virtual void closeConnections() = 0;

        /// \brief begin transaction
        virtual std::shared_ptr<ITransaction> openTransaction() = 0;

        // TODO: a set of 'operators' which are about to replace methods above
        //       in the name of interface segregation and repository pattern (see #272 on github)

        /**
         * \brief get group operator
         * \return group operator
         */
        virtual IGroupOperator& groupOperator() = 0;

        /**
         * \brief get photo operator
         * \return photo operator
         */
        virtual IPhotoOperator& photoOperator() = 0;

        /**
         * \brief get changelog operator
         * \return changelog operator
         */
        virtual IPhotoChangeLogOperator& photoChangeLogOperator() = 0;

        virtual IPeopleInformationAccessor& peopleInformationAccessor() = 0;

    signals:
        /// emited after new photos were added to database
        void photosAdded(const std::vector<Photo::Id> &);

        ///< emited when photos updated
        void photosModified(const std::set<Photo::Id> &);

        ///< emited after photos removal
        void photosRemoved(const std::vector<Photo::Id> &);

        ///< emited when done with photos marking
        void photosMarkedAsReviewed(const std::vector<Photo::Id> &);

    private:
        Q_OBJECT
    };
}


inline void DbErrorOnFalse(bool condition,
                           Database::StatusCodes ERRCODE = Database::StatusCodes::GeneralError,
                           const std::string& details = std::string(),
                           const std::source_location& location = std::source_location::current())
{
    if (condition == false)
        throw Database::db_error(std::string(location.file_name()) + ":" + std::to_string(location.line()) + " " + __PRETTY_FUNCTION__, ERRCODE, details);
}

inline void DbErrorOnFalse(Database::BackendStatus status,
                           const std::string& details = std::string(),
                           const std::source_location& location = std::source_location::current())
{
    DbErrorOnFalse(static_cast<bool>(status), status, details, location);
}

#endif
