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

#ifndef SERIESDETECTION_HPP
#define SERIESDETECTION_HPP

#include <QDialog>
#include <QQmlPropertyMap>

#include <database/photo_data.hpp>

#include "utils/ithumbnails_manager.hpp"
#include "quick_views/qml_setup.hpp"
#include "models/series_model.hpp"


class QStandardItemModel;

namespace Database
{
    struct IDatabase;
    struct IBackend;
}

class Project;
struct ICoreFactoryAccessor;
struct IThumbnailsManager;

class SeriesDetection: public QDialog
{
        Q_OBJECT

    public:
        SeriesDetection(Database::IDatabase &, ICoreFactoryAccessor *, IThumbnailsManager *, Project &);
        ~SeriesDetection();

    private:
        SeriesModel m_seriesModel;
        ICoreFactoryAccessor* m_core;
        Database::IDatabase& m_db;
        Project& m_project;
        QQuickWidget* m_qmlView;
        QML_IThumbnailsManager m_thumbnailsManager4QML;

        void fetch_series(Database::IBackend &);
        void load_series(const std::vector<GroupCandidate> &);
};

#endif // SERIESDETECTION_HPP
