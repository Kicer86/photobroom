/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../signal_mapper.hpp"

#include <idatabase.hpp>


namespace Database
{

    SignalMapper::SignalMapper(IDatabase* db): m_db(nullptr)
    {
        set(db);
    }


    void SignalMapper::set(IDatabase* db)
    {
        if (m_db != nullptr)
            m_db->backend()->disconnect(this);

        if (db != nullptr)
        {
            IBackend* backend = db->backend();
            assert(backend);

            // conversion required (use Qt::DirectConnection to be sure we were called from IBackend's thread)
            connect(backend, &IBackend::photosAdded, this, &SignalMapper::i_photosAdded, Qt::DirectConnection);
            connect(backend, &IBackend::photoModified, this, &SignalMapper::i_photoModified, Qt::DirectConnection);

            // direct reemits
            connect(backend, &IBackend::photosRemoved, this, &SignalMapper::photosRemoved);
            connect(backend, &IBackend::photosMarkedAsReviewed, this, &SignalMapper::photosMarkedAsReviewed);
        }

        m_db = db;
    }


    void SignalMapper::i_photosAdded(const std::vector<Photo::Id>& ids) const
    {
        std::vector<IPhotoInfo::Ptr> photos;
        photos.reserve(ids.size());

        for (const Photo::Id& id: ids)
        {
            IPhotoInfo::Ptr photo = m_db->utils()->getPhotoFor(id);
            photos.push_back(photo);
        }

        emit photosAdded(photos);
    }


    void SignalMapper::i_photoModified(const Photo::Id& id) const
    {
        IPhotoInfo::Ptr photo = m_db->utils()->getPhotoFor(id);
        emit photoModified(photo);
    }

}
