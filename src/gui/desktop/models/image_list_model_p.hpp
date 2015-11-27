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

#ifndef IMAGELISTMODELPRIVATE_HPP
#define IMAGELISTMODELPRIVATE_HPP

#include <deque>

#include <QObject>
#include <QIcon>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <OpenLibrary/putils/ts_queue.hpp>

#include <core/callback_ptr.hpp>
#include <core/itask_executor.hpp>

struct IPhotosManager;
struct ImageListModel;

struct Info
{
    QString path;
    QImage image;
    QString filename;
    std::size_t row;
    bool default_image;

    Info(const QString& p, const QImage& defaultImage, std::size_t r): path(p), image(defaultImage), filename(), row(r), default_image(true)
    {

    }
};


using boost::multi_index::multi_index_container;
using boost::multi_index::indexed_by;
using boost::multi_index::ordered_unique;
using boost::multi_index::member;

class ImageListModelPrivate: public QObject
{
        Q_OBJECT

        typedef multi_index_container<
            Info,
            indexed_by<
                ordered_unique<member<Info, QString, &Info::path> >,
                ordered_unique<member<Info, std::size_t, &Info::row> >
            >
        > InfoSet;

    public:
        ImageListModelPrivate(ImageListModel* q);
        ImageListModelPrivate(const ImageListModelPrivate &) = delete;
        virtual ~ImageListModelPrivate();

        ImageListModelPrivate& operator=(const ImageListModelPrivate &) = delete;

        QModelIndex get(const Info &) const;

        InfoSet m_data;
        std::recursive_mutex m_data_mutex;

        ITaskExecutor::TaskQueue m_taskQueue;
        callback_ptr_ctrl<ImageListModelPrivate> m_callback_ctrl;
        QImage m_image;
        IPhotosManager* m_photosManager;

    private:
        class ImageListModel* const q;


    signals:
        void imageScaled(const QString &, const QImage &);
};

#endif // IMAGELISTMODELPRIVATE_HPP
