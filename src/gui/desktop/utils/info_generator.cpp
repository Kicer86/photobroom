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

namespace std
{
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args... args)
    {
        return std::unique_ptr<T>( new T(args...) );
    }
}

namespace
{
    struct CollectionPhotos: Database::AGetPhotosCount
    {
        virtual void got(int c)
        {

        }
    };


    struct StagedAreaPhotos: Database::AGetPhotosCount
    {
        virtual void got(int c)
        {

        }
    };
}


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
    m_signalFiler->connect(notifier, SIGNAL(photoChanged(IPhotoInfo::Ptr)), this, SLOT(dbChanged()));
}


void InfoGenerator::dbChanged()
{
    auto stagedAreaPhotos = std::make_unique<StagedAreaPhotos>();
    auto collectionPhotos = std::make_unique<CollectionPhotos>();

    auto stagedAreaPhotosFilter = std::make_shared<Database::FilterPhotosWithFlags>();
    auto collectionPhotosFilter = std::make_shared<Database::FilterPhotosWithFlags>();

    stagedAreaPhotosFilter->flags[IPhotoInfo::FlagsE::StagingArea] = 1;
    collectionPhotosFilter->flags[IPhotoInfo::FlagsE::StagingArea] = 0;

    m_database->exec(std::move(stagedAreaPhotos), {stagedAreaPhotosFilter});
    m_database->exec(std::move(collectionPhotos), {collectionPhotosFilter});


    /*
    QString infoText;

    if (m_currentPrj.get() == nullptr)
        infoText = tr("No photo collection is opened.\n\n"
        "Use 'open' action form 'Photo collection' menu to choose one\n"
        "or 'new' action and create new collection.");

    const bool photos_in_staging_area = m_stagedImagesModel->isEmpty() == false;
    const bool photos_in_images_area  = m_imagesModel->isEmpty() == false;
    const bool photos_collector_works = m_photosCollector->isWorking();

    const bool state_photos_for_review = photos_in_images_area == false && (photos_in_staging_area || photos_collector_works);

    if (infoText.isEmpty() && state_photos_for_review)
        infoText = tr("%1.\n\n"
        "All new photos are added to special area where they can be reviewed before they will be added to collection.\n"
        "To se those photos choose %2 and then %3\n")
        .arg(photos_collector_works? tr("Photos are being loaded"): tr("Photos waiting for review"))
        .arg(ui->menuWindows->title())
        .arg(m_views[1]->getName());

    if (infoText.isEmpty() && m_imagesModel->isEmpty() == 0)
        infoText = tr("There are no photos in your collection.\n\nAdd some by choosing 'Add photos' action from 'Photos' menu.");

    if (infoText.isEmpty() == false)
        ui->infoWidget->setText(infoText);

    if (infoText.isEmpty() && ui->infoWidget->isVisible())
        ui->infoWidget->hide();

    if (infoText.isEmpty() == false && ui->infoWidget->isHidden())
        ui->infoWidget->show();
    */
}
