/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "synchronous_database.hpp"


SynchronousDatabase::SynchronousDatabase()
{

}


SynchronousDatabase::~SynchronousDatabase()
{

}


void SynchronousDatabase::closeConnections()
{

}


void SynchronousDatabase::getAllPhotos(const Database::Task&)
{

}


void SynchronousDatabase::getPhoto(const Database::Task&, const PhotoInfo::Id&)
{

}


void SynchronousDatabase::getPhotos(const Database::Task&, const std::deque< Database::IFilter::Ptr >&)
{

}


bool SynchronousDatabase::init(const Database::Task&, const std::string&)
{

}


void SynchronousDatabase::listTags(const Database::Task&)
{

}


void SynchronousDatabase::listTagValues(const Database::Task&, const TagNameInfo&)
{

}


void SynchronousDatabase::listTagValues(const Database::Task&, const TagNameInfo&, const std::deque< Database::IFilter::Ptr >&)
{

}


Database::ADatabaseSignals* SynchronousDatabase::notifier()
{

}


Database::Task SynchronousDatabase::prepareTask(Database::IDatabaseClient*)
{

}


void SynchronousDatabase::store(const Database::Task&, const PhotoInfo::Ptr&)
{

}
