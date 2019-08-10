/*
 * LazyTreeItemDelegate - loads images from external source
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

#include "lazy_tree_item_delegate.hpp"

#include <QPainter>

#include <core/down_cast.hpp>
#include <core/jobs_manager.hpp>
#include <core/media_types.hpp>
#include <core/task_executor_utils.hpp>
#include <database/igroup_operator.hpp>

#include "models/aphoto_info_model.hpp"
#include "utils/ithumbnail_acquisitor.hpp"
#include "utils/groups_manager.hpp"
#include "utils/painter_helpers.hpp"


LazyTreeItemDelegate::LazyTreeItemDelegate(ImagesTreeView* view):
    TreeItemDelegate(view),
    m_thumbnailAcquisitor(),
    m_groupCache(1024)
{

}


LazyTreeItemDelegate::~LazyTreeItemDelegate()
{

}


void LazyTreeItemDelegate::set(IThumbnailAcquisitor* acquisitor)
{
    m_thumbnailAcquisitor = acquisitor;
}


void LazyTreeItemDelegate::set(Database::IDatabase* db)
{
    m_db = db;
}


QImage LazyTreeItemDelegate::getImage(const QModelIndex& idx, const QSize& size) const
{
    const QAbstractItemModel* model = idx.model();
    const APhotoInfoModel* photoInfoModel = down_cast<const APhotoInfoModel*>(model);      // TODO: not nice (see issue #177)
    const Photo::Data& details = photoInfoModel->getPhotoDetails(idx);

    const ThumbnailInfo info = { details.path, size.height() };
    QImage image = m_thumbnailAcquisitor->getThumbnail(info);

    QString text;

    if (details.groupInfo.role == GroupInfo::Representative)
    {
        const Group::Type type = getGroupTypeFor(details.groupInfo.group_id);
        switch(type)
        {
            case Group::Animation:
                text = tr("stop motion");
                break;

            case Group::HDR:
                text = "HDR";
                break;

            case Group::Generic:
                text = tr("Group");
                break;

            case Group::Invalid:
                break;
        }
    }

    if (MediaTypes::isVideoFile(details.path))
        text = "VID";

    if (text.isNull() == false)
    {
        const QPen outline(Qt::black);
        const QPen textColor(Qt::white);

        QPainter painter(&image);

        const QFont font = painter.font();
        const QFontMetrics fontMetrics(font, painter.device());
        QRect bb = fontMetrics.boundingRect(text);
        bb.moveTop(0);
        const QPoint sp = QPoint(bb.left(), fontMetrics.height()) + QPoint(5, 0);

        painter.setPen(textColor);
        PainterHelpers::drawTextWithOutline(&painter, sp, text, outline);
    }

    return image;
}


Group::Type LazyTreeItemDelegate::getGroupTypeFor(const Group::Id& gid) const
{
    Group::Type* grpType = m_groupCache.object(gid);;

    if (grpType == nullptr)
    {
        // TODO: figure out how to get rid of this ugly cast
        LazyTreeItemDelegate* pThis = const_cast<LazyTreeItemDelegate *>(this);

        // get type from db and store in cache.

        /*  TODO: fix it one day, when template master arrives
        job(m_db, [gid](Database::IBackend* backend)
        {
            const Group::Type type = backend->groupOperator()->type(gid);

            return type;
        }).execute2<const Group::Type &>(pThis, [gid, cache = this->m_groupCache](const Group::Type& type)
        {
            // update cache
            auto locked_cache = cache->lock();
            locked_cache->insert(gid, new Group::Type(type));
        });
        */

        // TODO: make_cross_thread_function should guess function's args
        auto callback = make_cross_thread_function<const Group::Type &>(pThis, [gid, this](const Group::Type& type)
        {
            // update cache
            m_groupCache.insert(gid, new Group::Type(type));
        });

        m_db->exec([gid, callback](Database::IBackend* backend)
        {
            const Group::Type type = backend->groupOperator()->type(gid);

            callback(type);
        });
    }

    return grpType == nullptr? Group::Type::Invalid: *grpType;
}
