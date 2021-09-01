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
        std::lock_guard<std::mutex> guard(m_dbMutex);

        if (m_db != nullptr)
            disconnect(&m_db->backend(), &IBackend::photosModified, this, &SignalMapper::i_photosModified);

        if (db != nullptr)
        {
            IBackend& backend = db->backend();

            // conversion required (use Qt::DirectConnection to be sure we were called from IBackend's thread)
            connect(&backend, &IBackend::photosModified, this, &SignalMapper::i_photosModified, Qt::DirectConnection);
        }

        m_db = db;
    }


    void SignalMapper::i_photosModified(const std::set<Photo::Id>& ids) const
    {
        std::lock_guard<std::mutex> guard(m_dbMutex);

        auto& utils = m_db->utils();

        for (const Photo::Id& id: ids)
        {
            IPhotoInfo::Ptr photo = utils.getPhotoFor(id);
            emit photoModified(photo);
        }
    }

}
