/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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

#include "series_detection.hpp"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QQuickWidget>

#include <core/icore_factory_accessor.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>
#include <database/idatabase.hpp>

#include "ui/photos_grouping_dialog.hpp"
#include "quick_views/qml_utils.hpp"

Q_DECLARE_METATYPE(SeriesDetector::GroupCandidate)

using namespace std::placeholders;

namespace
{
    constexpr int DetailsRole = Qt::UserRole + 1;
    constexpr int PropertiesRole = DetailsRole + 1;
    constexpr int thumbnail_size = 64;
    const QString loadedPropertyName("loaded");
}

SeriesDetection::SeriesDetection(Database::IDatabase* db,
                                 ICoreFactoryAccessor* core,
                                 IThumbnailsManager* thbMgr,
                                 Project* project):
    QDialog(),
    m_tabModel(new QStandardItemModel(this)),
    m_core(core),
    m_db(db),
    m_project(project),
    m_thumbnailsManager4QML(thbMgr)
{
    // dialog top layout setup
    resize(320, 480);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* buttons_layout = new QHBoxLayout;
    QDialogButtonBox* dialog_buttons = new QDialogButtonBox(QDialogButtonBox::Close);

    // NOTE: https://machinekoder.com/creating-qml-properties-dynamically-runtime-c/
    m_modelDynamicProperties.insert(loadedPropertyName, false);

    m_tabModel->setItemRoleNames( {{PropertiesRole, "photoProperties"}} );

    auto view = new QQuickWidget(this);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QmlUtils::registerObject(view, "thumbnailsManager", &m_thumbnailsManager4QML);
    QmlUtils::registerObject(view, "groupsModelId", m_tabModel);
    QmlUtils::registerObjectProperties(view, "groupsModelState", &m_modelDynamicProperties);
    view->setSource(QUrl("qrc:/ui/SeriesDetection.qml"));

    layout->addWidget(view);
    layout->addLayout(buttons_layout);
    layout->addWidget(dialog_buttons);

    // buttons
    QPushButton* group_button = new QPushButton(tr("Group"), this);
    group_button->setDisabled(true);
    buttons_layout->addWidget(group_button);
    buttons_layout->addStretch();

    // wiring
    connect(dialog_buttons, &QDialogButtonBox::rejected, this, &QDialog::accept);
    connect(group_button, &QPushButton::pressed, this, &SeriesDetection::group);

    auto callback = m_callback_mgr.make_safe_callback<Database::IBackend &>(std::bind(&SeriesDetection::fetch_series, this, _1));
    m_db->exec(callback);
}


SeriesDetection::~SeriesDetection()
{
    m_callback_mgr.invalidate();
}


void SeriesDetection::fetch_series(Database::IBackend& backend)
{
    IExifReaderFactory* exif = m_core->getExifReaderFactory();
    SeriesDetector detector(backend, exif->get());

    const auto candidates = detector.listCandidates();

    // go back to main thread
    invokeMethod(this, &SeriesDetection::load_series, candidates);
}


void SeriesDetection::load_series(const std::vector<SeriesDetector::GroupCandidate>& candidates)
{
    for(std::size_t i = 0; i < candidates.size(); i++)
    {
        const SeriesDetector::GroupCandidate& candidate = candidates[i];
        const Photo::Data& representativeData = candidate.members.front();

        QList<QStandardItem *> row;

        QString type;
        switch (candidate.type)
        {
            case Group::Type::Invalid:                           break;
            case Group::Type::Animation: type = tr("Animation"); break;
            case Group::Type::HDR:       type = tr("HDR");       break;
            case Group::Type::Generic:   type = tr("Generic");   break;
        }

        QStandardItem* groupItem = new QStandardItem;
        groupItem->setData(QVariant::fromValue(representativeData), PropertiesRole);

        row.append(groupItem);

        m_tabModel->appendRow(row);
    }

    m_modelDynamicProperties.insert(loadedPropertyName, true);
}


void SeriesDetection::group()
{
    const int row = selected_row();
    const QModelIndex firstItemInRow = m_tabModel->index(row, 0);
    const SeriesDetector::GroupCandidate groupDetails = firstItemInRow.data(DetailsRole).value<SeriesDetector::GroupCandidate>();
    launch_groupping_dialog(groupDetails.members, groupDetails.type);
}


void SeriesDetection::launch_groupping_dialog(const std::vector<Photo::Data>& ph_data, Group::Type type)
{
    auto logger = m_core->getLoggerFactory()->get("PhotosGrouping");

    PhotosGroupingDialog pgd(
        ph_data,
        m_core->getExifReaderFactory(),
        m_core->getTaskExecutor(),
        m_core->getConfiguration(),
        logger.get(),
        type
    );
    const int exit_code = pgd.exec();

    if (exit_code == QDialog::Accepted)
    {
        PhotosGroupingDialogUtils::createGroup(&pgd, m_project, m_db);

        const int row = selected_row();
        m_tabModel->removeRow(row);
    }
}


int SeriesDetection::selected_row() const
{
    return 0;
}
