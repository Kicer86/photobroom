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
#include <QJSValueIterator>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QQmlEngine>
#include <QQuickWidget>

#include <core/icore_factory_accessor.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>
#include <database/idatabase.hpp>

#include "ui/photos_grouping_dialog.hpp"
#include "quick_views/qml_utils.hpp"
#include "utils/groups_manager.hpp"


Q_DECLARE_METATYPE(GroupCandidate)

using namespace std::placeholders;


SeriesDetection::SeriesDetection(Database::IDatabase& db,
                                 ICoreFactoryAccessor* core,
                                 ITasksView& tasksView,
                                 Project& project,
                                 IThumbnailsManager& thbMgr):
    QDialog(),
    m_seriesModel(project, *core, tasksView),
    m_core(core),
    m_db(db),
    m_project(project),
    m_qmlView(nullptr)
{
    // dialog top layout setup
    resize(320, 480);

    QVBoxLayout* layout = new QVBoxLayout(this);
    QDialogButtonBox* dialog_buttons = new QDialogButtonBox(QDialogButtonBox::Close);

    m_qmlView = new QQuickWidget(this);
    m_qmlView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_qmlView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QmlUtils::registerObject(m_qmlView, "groupsModelId", &m_seriesModel);
    QmlUtils::registerImageProviders(m_qmlView, thbMgr);
    m_qmlView->setSource(QUrl("qrc:/ui/Views/SeriesDetection.qml"));

    layout->addWidget(m_qmlView);
    layout->addWidget(dialog_buttons);

    connect(dialog_buttons, &QDialogButtonBox::rejected, this, &QDialog::accept);
}


SeriesDetection::~SeriesDetection()
{
    // delete qml view before all other objects it referes to will be deleted
    delete m_qmlView;
}
