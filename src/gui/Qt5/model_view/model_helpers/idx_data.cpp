/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "idx_data.hpp"

#include <assert.h>

#include <QPixmap>
#include <QVariant>

#include <core/photo_info.hpp>

#include "idx_data_manager.hpp"

IdxData::IdxData(IdxDataManager* model, IdxData* parent, const QString& name) : IdxData(model, parent)
{
    m_data[Qt::DisplayRole] = name;
}


IdxData::IdxData(IdxDataManager* model, IdxData* parent, const PhotoInfo::Ptr& photo) : IdxData(model, parent)
{
    m_photo = photo;
    m_loaded = FetchStatus::Fetched;

    updateLeafData();
    photo->registerObserver(this);
}


IdxData::~IdxData()
{
    m_model->idxDataDeleted(this);
    if (m_photo.get() != nullptr)
        m_photo->unregisterObserver(this);

    reset();
}


void IdxData::setNodeData(const Database::IFilter::Ptr& filter)
{
    m_filter = filter;
}


void IdxData::addChild(IdxData* child)
{
    assert(m_photo.get() == nullptr);             //child (leaf) cannot accept any child

    child->setPosition(m_children.size(), 0);
    m_children.push_back(child);
}


void IdxData::reset()
{
    m_model->idxDataReset(this);
    m_loaded = FetchStatus::NotFetched;
    for(IdxData* child: m_children)      //TODO: it may be required to move deletion to another thread (slow deletion may impact gui)
        delete child;

    m_children.clear();
    m_photo.reset();
    m_data.clear();
}


IdxData::IdxData(IdxDataManager* model, IdxData* parent) :
    m_children(),
    m_data(),
    m_filter(new Database::FilterEmpty),
    m_photo(nullptr),
    m_parent(parent),
    m_model(model),
    m_level(-1),
    m_row(-1),
    m_column(-1),
    m_loaded(FetchStatus::NotFetched)
{
    m_level = parent ? parent->m_level + 1 : 0;
    m_model->idxDataCreated(this);
}


void IdxData::setPosition(int row, int col)
{
    m_row = row;
    m_column = col;
}


void IdxData::updateLeafData()
{
    m_data[Qt::DisplayRole] = m_photo->getPath();
    m_data[Qt::DecorationRole] = m_photo->getThumbnail();
}


void IdxData::photoUpdated(PhotoInfo *)  //parameter not used as we have only one photo
{
    updateLeafData();
}
