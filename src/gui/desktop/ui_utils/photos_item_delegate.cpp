/*
 * PhotosItemDelegate - extension of TreeItemDelegate
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

#include "photos_item_delegate.hpp"

#include <QPainter>

#include <core/down_cast.hpp>
#include <core/iconfiguration.hpp>

#include "config_keys.hpp"
#include "utils/config_tools.hpp"
#include "utils/ithumbnail_acquisitor.hpp"

#include "models/db_data_model.hpp"


PhotosItemDelegate::PhotosItemDelegate(ImagesTreeView* view, IConfiguration* config):
    LazyTreeItemDelegate(view),
    m_config(config)
{
    readConfig();
}


PhotosItemDelegate::~PhotosItemDelegate()
{

}


void PhotosItemDelegate::set(IConfiguration* config)
{
    m_config = config;
    m_config->registerObserver(this);

    readConfig();
}


void PhotosItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    LazyTreeItemDelegate::paint(painter, option, index);

    const bool node = (option.features & QStyleOptionViewItem::HasDecoration) == 0;

    if (node)
    {
        // decorate node with its status
        const QAbstractItemModel* m = index.model();

        //TODO: ugly casting ! Issue #177 on github
        const DBDataModel* model = down_cast<const DBDataModel *>(m);
        const QVariant statusVariant = model->data(index, DBDataModel::NodeStatus);

        assert(statusVariant.canConvert<int>());
        const NodeStatus status = static_cast<NodeStatus>(statusVariant.toInt());

        QString state;

        switch(status)
        {
            case NodeStatus::Fetching:
                state = tr("Loading");
                break;

            case NodeStatus::Fetched:
            {
                const bool children = m->hasChildren(index);
                if (children == false)
                    state = tr("No photos");
                break;
            }

            case NodeStatus::NotFetched:
                break;
        }

        if (state.isEmpty() == false)
        {
            const QRect& r = option.rect;
            painter->drawText(r.x() + 10, r.y() + 17, state);
        }
    }
}


void PhotosItemDelegate::readConfig()
{
    if (m_config != nullptr)
    {
        setupEvenColor(m_config->getEntry(ViewConfigKeys::bkg_color_even));
        setupOddColor(m_config->getEntry(ViewConfigKeys::bkg_color_odd));
    }
}


void PhotosItemDelegate::setupEvenColor(const QVariant& v)
{
    const uint32_t evenColor = v.toUInt();
    const QColor evenQColor = ConfigTools::intToColor(evenColor);

    setNodeBackgroundEvenColor(evenQColor);
}


void PhotosItemDelegate::setupOddColor(const QVariant& v)
{
    const uint32_t oddColor = v.toUInt();
    const QColor oddQColor = ConfigTools::intToColor(oddColor);

    setNodeBackgroundOddColor(oddQColor);
}


void PhotosItemDelegate::configChanged(const QString& entry, const QVariant& value)
{
    if (entry == ViewConfigKeys::bkg_color_even)
        setupEvenColor(value);
    else if (entry == ViewConfigKeys::bkg_color_odd)
        setupOddColor(value);
}
