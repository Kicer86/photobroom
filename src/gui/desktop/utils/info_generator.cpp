/*
 * Generator for info and hints.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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
 *
 */

#include "info_generator.hpp"

#include <database/idatabase.hpp>

#include "signal_filter.hpp"


InfoGenerator::InfoGenerator(QObject* parent_object): QObject(parent_object), m_database(nullptr), m_signalFiler(new SignalFilter(this))
{

}


InfoGenerator::~InfoGenerator()
{

}


void InfoGenerator::set(Database::IDatabase* database)
{
    m_database = database;

    auto notifier = m_database->notifier();
    m_signalFiler->connect(notifier, SIGNAL(photoAdded(IPhotoInfo::Ptr)), this, SLOT(dbChanged()));
}


void InfoGenerator::dbChanged()
{

}
