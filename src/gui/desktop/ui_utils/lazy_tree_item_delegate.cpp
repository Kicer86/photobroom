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


template<typename T>
struct ExecutorTraits<Database::IDatabase, T>
{
    static void exec(Database::IDatabase* db, T&& t)
    {
        db->exec(std::forward<T>(t));
    }
};


LazyTreeItemDelegate::LazyTreeItemDelegate(ImagesTreeView* view):
    TreeItemDelegate(view),
    m_thumbnailAcquisitor(),
    m_groupCache(std::make_shared<TS_Cache>(1024))
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
                text = "gif";
                break;

            case Group::HDR:
                text = "HDR";
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
    Group::Type* grpType = nullptr;
    {
        auto locked_cache = m_groupCache->lock();

        grpType = locked_cache->object(gid);
    }

    if (grpType == nullptr)
    {
        // get type from db and store in cache.
        job(m_db, [gid, cache = this->m_groupCache](Database::IBackend* backend)
        {
            const Group::Type type = backend->groupOperator()->type(gid);

            // update cache
            auto locked_cache = cache->lock();
            locked_cache->insert(gid, new Group::Type(type));

            return 0;
        }).wait<int(Database::IBackend*)>();
    }

    return grpType == nullptr? Group::Type::Invalid: *grpType;
}
