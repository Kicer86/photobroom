/*
 * Photo analyzer and updater.
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

#include "../photos_analyzer.hpp"
#include "photos_analyzer_p.hpp"


#include <OpenLibrary/putils/ts_resource.hpp>



///////////////////////////////////////////////////////////////////////////////

void IncompletePhotos::got(const IPhotoInfo::List& photos)
{
    for(const IPhotoInfo::Ptr& photo: photos)
        m_analyzerImpl->addPhoto(photo);
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////


PhotosAnalyzer::PhotosAnalyzer(): m_data(new PhotosAnalyzerImpl)
{

}


PhotosAnalyzer::~PhotosAnalyzer()
{
    delete m_data, m_data = nullptr;
}


void PhotosAnalyzer::setDatabase(Database::IDatabase* new_database)
{
    //disconnect current database
    Database::IDatabase* cur_database = m_data->getDatabase();

    if (cur_database)
    {
        Database::ADatabaseSignals* notifier = cur_database->notifier();

        notifier->disconnect(this);
    }

    //setup new database
    m_data->setDatabase(new_database);

    //and new connections
    if (new_database)
    {
        Database::ADatabaseSignals* notifier = new_database->notifier();

        connect(notifier, SIGNAL(photoAdded(IPhotoInfo::Ptr)), this, SLOT(photoAdded(IPhotoInfo::Ptr)), Qt::DirectConnection);
    }
}


void PhotosAnalyzer::set(ITaskExecutor* taskExecutor)
{
    m_data->set(taskExecutor);
}


void PhotosAnalyzer::set(IConfiguration* configuration)
{
    m_data->set(configuration);
}


void PhotosAnalyzer::set(ITasksView* tasksView)
{
    m_data->set(tasksView);
}


void PhotosAnalyzer::stop()
{
    m_data->stop();
}


void PhotosAnalyzer::photoAdded(const IPhotoInfo::Ptr& photo)
{
    m_data->addPhoto(photo);
}
