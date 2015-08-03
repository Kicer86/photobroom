/*
 * Flat list model with lazy image load.
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

#include "image_list_model.hpp"
#include "image_list_model_p.hpp"

#include <QFileInfo>

#include <core/itask_executor.hpp>
#include <core/callback_ptr.hpp>


struct LoadPhoto: ITaskExecutor::ITask
{
    LoadPhoto(const QString& path, const callback_ptr_ctrl<ImageListModelPrivate>& callback_ctrl): m_path(path), m_callback(callback_ctrl)
    {

    }

    virtual std::string name() const
    {
        return "LoadPhoto";
    }

    virtual void perform()
    {
        // TODO: remove constants, use settings?
        const QPixmap pixmap(m_path);
        
        const int w = 1920;
        const int h = 1080;

        const bool needs_resize = pixmap.width() > w || pixmap.height() > h;

        const QPixmap scaled = needs_resize?
                               pixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation):
                               pixmap;

        auto callback = **m_callback;

        if (callback)
            callback->imageScaled(m_path, scaled);
    }

    QString m_path;
    callback_ptr<ImageListModelPrivate> m_callback;
};


///////////////////////////////////////////////////////////////////////////////


ImageListModelPrivate::ImageListModelPrivate(ImageListModel* q):
    q(q),
    m_data(),
    m_taskExecutor(nullptr),
    m_callback_ctrl(this)
{
    qRegisterMetaType<QVector<int>>("QVector<int>");
}


ImageListModelPrivate::~ImageListModelPrivate()
{

}


void ImageListModelPrivate::imageScaled(const QString& path, const QPixmap& pixmap)
{
    q->imageScaled(path, pixmap);
}

///////////////////////////////////////////////////////////////////////////////


ImageListModel::ImageListModel(QObject* p):
    QAbstractItemModel(p),
    d(new ImageListModelPrivate(this))
{

}


ImageListModel::ImageListModel(const ImageListModel& other)
    : d(new ImageListModelPrivate(this))
{

}


ImageListModel::~ImageListModel()
{
    delete d;
}


ImageListModel& ImageListModel::operator=(const ImageListModel& other)
{

}


bool ImageListModel::operator==(const ImageListModel& other) const
{

}


void ImageListModel::insert(const QString& path)
{
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);
    const int s = d->m_data.size();

    beginInsertRows(QModelIndex(), s, s);

    d->m_data.push_back(path);

    endInsertRows();
}


void ImageListModel::clear()
{
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);

    beginResetModel();

    d->m_callback_ctrl.invalidate();
    d->m_data.clear();

    endResetModel();
}


void ImageListModel::set(ITaskExecutor* taskExecutor)
{
    d->m_taskExecutor = taskExecutor;
}


QVariant ImageListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);

    if (index.isValid())
    {
        Info& info = d->m_data[index.row()];

        switch(role)
        {
            case Qt::DisplayRole:
            {
                if (info.filename.isEmpty())
                {
                    QFileInfo file_info(info.path);
                    info.filename = file_info.fileName();
                }

                result = info.filename;
                break;
            }

            case Qt::DecorationRole:
            {
                if (info.icon.isNull())
                {
                    d->m_taskExecutor->add(std::make_unique<LoadPhoto>(info.path, d->m_callback_ctrl));
                }

                result = info.icon;
            }

            default:
                break;
        }
    }

    return result;
}


int ImageListModel::columnCount(const QModelIndex& parent) const
{
    const int result = parent.isValid()? 0 : 1;

    return result;
}


int ImageListModel::rowCount(const QModelIndex& parent) const
{
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);
    const int result = parent.isValid()? 0: d->m_data.size();

    return result;
}


QModelIndex ImageListModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}


QModelIndex ImageListModel::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column, nullptr);
}


void ImageListModel::imageScaled(const QString& path, const QPixmap& pixmap)
{
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);

    // TODO: not so smart huh?
    auto& data = d->m_data;

    std::size_t r = -1;
    for(std::size_t i = 0; i < data.size(); i++)
        if (data[i].path == path)
        {
            r = i;
            break;
        }

    assert(r != -1);

    Info& info = data[r];
    info.icon = pixmap;

    const QModelIndex idx = index(r, 0, QModelIndex());
    emit dataChanged(idx, idx, {Qt::DecorationRole});
}
