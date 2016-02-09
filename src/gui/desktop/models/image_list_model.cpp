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

#include <core/iphotos_manager.hpp>
#include <core/itask_executor.hpp>
#include <core/callback_ptr.hpp>


struct LoadPhoto: ITaskExecutor::ITask
{
    LoadPhoto(const QString& path,
              IPhotosManager* photosManager,
              const std::function< void(const QString &, const QImage &)>& callback):
        m_path(path),
        m_callback(callback),
        m_photosManager(photosManager)
    {

    }

    LoadPhoto(const LoadPhoto &) = delete;

    LoadPhoto& operator=(const LoadPhoto &) = delete;

    virtual std::string name() const
    {
        return "LoadPhoto";
    }

    virtual void perform()
    {
        QImage scaled = m_photosManager->getThumbnail(m_path);

        m_callback(m_path, scaled);
    }

    QString m_path;
    std::function<void(const QString &, const QImage &)> m_callback;
    IPhotosManager* m_photosManager;
};


///////////////////////////////////////////////////////////////////////////////


ImageListModelPrivate::ImageListModelPrivate(ImageListModel* _q):
    m_data(),
    m_data_mutex(),
    m_taskQueue(nullptr),
    m_callback_ctrl(),
    m_image(),
    m_photosManager(nullptr),
    q(_q)
{
    m_image = QImage(":/gui/clock-img.svg");
}


ImageListModelPrivate::~ImageListModelPrivate()
{

}


QModelIndex ImageListModelPrivate::get(const Info& data) const
{
    const QModelIndex idx = q->index(data.row, 0, QModelIndex());

    return idx;
}


///////////////////////////////////////////////////////////////////////////////


ImageListModel::ImageListModel(QObject* p):
    QAbstractItemModel(p),
    d(new ImageListModelPrivate(this))
{
    // make sure ImageListModel::imageScaled is called from main thread
    connect(d, &ImageListModelPrivate::imageScaled, this, &ImageListModel::imageScaled, Qt::QueuedConnection);
}


ImageListModel::~ImageListModel()
{
    delete d;
}


void ImageListModel::insert(const QString& path)
{
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);
    const int s = static_cast<int>( d->m_data.size() );

    beginInsertRows(QModelIndex(), s, s);

    d->m_data.insert( Info(path, d->m_image, s) );

    endInsertRows();
}


void ImageListModel::clear()
{
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);

    beginResetModel();

    d->m_taskQueue->clear();
    d->m_callback_ctrl.invalidate();
    d->m_data.clear();

    endResetModel();
}


const QString ImageListModel::get(const QModelIndex& idx) const
{
    QString result;

    if (idx.isValid())
    {
        auto infoIt = d->m_data.get<1>().find(idx.row());

        result = infoIt->path;
    }

    return result;
}


QModelIndex ImageListModel::get(const QString& path) const
{
    auto it = d->m_data.find(path);
    return it == d->m_data.end()? QModelIndex(): d->get(*it);
}


void ImageListModel::set(ITaskExecutor* taskExecutor)
{
    d->m_taskQueue = taskExecutor->getCustomTaskQueue();
}


void ImageListModel::set(IPhotosManager* photosManager)
{
    d->m_photosManager = photosManager;
}


QVariant ImageListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);

    if (index.isValid())
    {
        auto& data = d->m_data.get<1>();

        auto infoIt = data.find(index.row());
        Info info = *infoIt;

        switch(role)
        {
            case Qt::DisplayRole:
            {
                if (info.filename.isEmpty())
                {
                    QFileInfo file_info(info.path);
                    info.filename = file_info.fileName();

                    data.replace(infoIt, info);
                }

                result = info.filename;
                break;
            }

            case Qt::DecorationRole:
            {
                if (info.default_image)
                {
                    info.default_image = false;

                    auto callback = std::bind(&ImageListModelPrivate::imageScaled,
                                              d,
                                              std::placeholders::_1,
                                              std::placeholders::_2);

                    auto safe_callback = d->m_callback_ctrl.make_safe_callback< void(const QString &, const QImage &) >(callback);

                    d->m_taskQueue->push(std::make_unique<LoadPhoto>(info.path, d->m_photosManager, safe_callback));

                    data.replace(infoIt, info);
                }

                result = info.image;
                break;
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
    const int result = parent.isValid()? 0: static_cast<int>(d->m_data.size());

    return result;
}


QModelIndex ImageListModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}


QModelIndex ImageListModel::index(int row, int column, const QModelIndex& parent) const
{
    QModelIndex result;

    const bool valid = parent.isValid() == false && row < static_cast<int>(d->m_data.size()) && column == 0;

    if (valid)
        result = createIndex(row, column, nullptr);

    return result;
}


void ImageListModel::imageScaled(const QString& path, const QImage& image)
{
    std::lock_guard<std::recursive_mutex> lock(d->m_data_mutex);

    auto& data  = d->m_data.get<0>();
    auto it = data.find(path);

    assert(it != data.end());

    Info info = *it;
    info.image = image;
    d->m_data.replace(it, info);

    const QModelIndex idx = index(info.row, 0, QModelIndex());
    emit dataChanged(idx, idx, {Qt::DecorationRole});
}
