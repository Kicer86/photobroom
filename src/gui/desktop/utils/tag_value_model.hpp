/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef TAGVALUEMODEL_HPP
#define TAGVALUEMODEL_HPP

#include <QAbstractListModel>

#include <core/tag.hpp>
#include <core/imodel_compositor_data_source.hpp>

struct ILoggerFactory;
struct ITagInfoCollector;


class TagValueModel: public IModelCompositorDataSource
{
    public:
        TagValueModel(const std::set<Tag::Types> &, ITagInfoCollector *, ILoggerFactory *);
        TagValueModel(const TagValueModel &) = delete;
        ~TagValueModel();
        TagValueModel& operator=(const TagValueModel &) = delete;

        const QStringList& data() const override;

    private:
        QStringList m_values;
        std::set<Tag::Types> m_tagInfos;
        ITagInfoCollector* m_tagInfoCollector;
        ILoggerFactory* m_loggerFactory;

        void updateData();
        void collectorNotification(const Tag::Types &);
};

#endif // TAGVALUEMODEL_HPP
