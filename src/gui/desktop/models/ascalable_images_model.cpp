/*
 * Base for models offering scalable decoration role.
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

#include "ascalable_images_model.hpp"

#include <core/task_executor.hpp>

uint qHash(const AScalableImagesModel::Key& key)
{
    return qHash(key.index) + qHash(key.size.height()) + qHash(key.size.width());
}


struct LoadPhoto: ITaskExecutor::ITask
{
    LoadPhoto(const AScalableImagesModel::Key& key,
              const std::function< void(const AScalableImagesModel::Key &) >& callback):
    m_key(key),
    m_callback(callback)
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
        m_callback(m_key);
    }

    AScalableImagesModel::Key m_key;
    std::function< void(const AScalableImagesModel::Key &) > m_callback;
};


AScalableImagesModel::AScalableImagesModel(QObject* p):
    QAbstractItemModel(p),
    m_cache(100000),            // ~100kb
    m_callback_ctrl(),
    m_taskExecutor(nullptr)
{

}


AScalableImagesModel::~AScalableImagesModel()
{

}


QImage AScalableImagesModel::getDecorationRole(const QModelIndex& idx, const QSize& size)
{
    Key key(idx, size);

    QImage* img = m_cache.object(key);

    if (img == 0)
    {
        using namespace std::placeholders;

        auto callback = std::bind(&AScalableImagesModel::loadImage, this, _1);
        auto safe_callback = m_callback_ctrl.make_safe_callback< void(const AScalableImagesModel::Key &) >(callback);

        auto task = std::make_unique<LoadPhoto>(key, safe_callback);
        m_taskExecutor->add(std::move(task));
    }

    return *img;
}


void AScalableImagesModel::set(ITaskExecutor* taskExecutor)
{
    m_taskExecutor = taskExecutor;
}


void AScalableImagesModel::loadImage(const AScalableImagesModel::Key& key)
{
    const QImage scaled = getImageFor(key.index, key.size);

    // TODO: finish
}
