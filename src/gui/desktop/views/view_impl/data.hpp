/*
 * View's high level data structure
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

#ifndef DATA_HPP
#define DATA_HPP

#include <vector>
#include <functional>

#include <QRect>
#include <QModelIndex>

#include "model_index_info.hpp"
#include "view_data_set.hpp"

struct IConfiguration;
class APhotoInfoModel;


class Data
{
    public:
        typedef ViewDataSet<ModelIndexInfo> ModelIndexInfoSet;

        Data();
        Data(const Data &) = delete;

        ~Data();
        Data& operator=(const Data &) = delete;

        void set(APhotoInfoModel *);

        void setSpacing(int);
        void setImageMargin(int);
        void setThumbnailDesiredHeight(int);

        const ModelIndexInfo& get(const QModelIndex &) const;            // Same as find(), but has assert inside. Use when result is not expected to be invalid.
        ModelIndexInfo& get(const QModelIndex &);
        ModelIndexInfoSet::Model::iterator find(const QModelIndex &);

        ModelIndexInfoSet::Model::iterator get(const QPoint &) const;
        [[deprecated]] bool isImage(const ModelIndexInfoSet::Model::const_iterator &) const;
        bool isImage(const QModelIndex &) const;
        [[deprecated]] QPixmap getImage(Data::ModelIndexInfoSet::Model::const_iterator) const;
        [[deprecated]] QPixmap getImage(const QModelIndex &) const;
        [[deprecated]] QSize getImageSize(Data::ModelIndexInfoSet::Model::const_iterator) const;
        QSize getImageSize(const QModelIndex &) const;
        [[deprecated]] QSize getThumbnailSize(Data::ModelIndexInfoSet::Model::const_iterator) const;
        QSize getThumbnailSize(const QModelIndex &) const;
        void for_each_visible(std::function<bool(ModelIndexInfoSet::Model::iterator)>) const;
        [[deprecated]] QModelIndex get(ModelIndexInfoSet::Model::const_level_iterator) const;
        std::vector<QModelIndex> findInRect(const QRect &) const;

        [[deprecated]] bool isExpanded(const ModelIndexInfoSet::Model::const_iterator &) const;
        bool isExpanded(const QModelIndex &) const;
        [[deprecated]] bool isVisible(const ModelIndexInfoSet::Model::const_level_iterator &) const;
        bool isVisible(const QModelIndex &) const;

        const ModelIndexInfoSet& getModel() const;
        ModelIndexInfoSet& getModel();

        int getSpacing() const;
        int getImageMargin() const;
        int getThumbnailDesiredHeight() const;

        IConfiguration* getConfig();

        //getting siblings
        QModelIndex getRightOf(const QModelIndex &) const;
        QModelIndex getLeftOf(const QModelIndex &) const;
        QModelIndex getTopOf(const QModelIndex &) const;
        QModelIndex getBottomOf(const QModelIndex &) const;

        QModelIndex getFirst(const QModelIndex &) const;
        QModelIndex getLast(const QModelIndex &) const;

    private:
        std::unique_ptr<ModelIndexInfoSet> m_itemData;
        APhotoInfoModel* m_model;
        IConfiguration* m_configuration;
        int m_spacing;
        int m_margin;
        int m_thumbHeight;

        std::vector<QModelIndex> findInRect(ModelIndexInfoSet::Model::const_level_iterator, ModelIndexInfoSet::Model::const_level_iterator, const QRect &) const;
};

#endif // DATA_HPP
