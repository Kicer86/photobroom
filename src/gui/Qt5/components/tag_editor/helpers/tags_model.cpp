/*
 * Tags model
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

/// TODO: remove
#if defined _MSC_VER
    #if _MSC_VER >= 1800
        #define Q_COMPILER_INITIALIZER_LISTS
    #else
        #error unsupported compiler
    #endif
#endif


#include "tags_model.hpp"

#include <QItemSelectionModel>

#include <core/tag_updater.hpp>
#include "model_view/db_data_model.hpp"


struct TagGroupUpdater
{
        TagGroupUpdater(const std::vector<IPhotoInfo::Ptr>& photos): m_tagUpdaters()
        {
            for(const IPhotoInfo::Ptr& photo: photos)
                m_tagUpdaters.emplace_back(photo);
        }

        Tag::TagsList getTags() const
        {
            Tag::TagsList tags;

            for(const TagUpdater& tagUpdater: m_tagUpdaters)
            {
                const Tag::TagsList l_tags = tagUpdater.getTags();

                for(auto it = l_tags.begin(); it != l_tags.end(); ++it)
                {
                    auto f_it = tags.find(it->first);      //check if this tag already exists in main set of tags

                    if (f_it != tags.end())  //it does
                    {
                        //check if values are the same
                        Tag::Info info_it(it);
                        Tag::Info info_f_it(f_it);
                        if (info_it.valuesString() != info_f_it.valuesString())
                        {
                            Tag::ValuesSet new_value( { TagValueInfo("<multiple values>") } );
                            f_it->second = new_value;
                        }
                    }
                    else
                        tags.insert(*it);
                }
            }

            return tags;
        }

        void setTag(const TagNameInfo& name, const Tag::ValuesSet& values)
        {
            for (TagUpdater& updater: m_tagUpdaters)
                updater.setTag(name, values);
        }

        void setTag(const TagNameInfo& name, const TagValueInfo& value)
        {
            for (TagUpdater& updater: m_tagUpdaters)
                updater.setTag(name, value);
        }

        void setTags(const Tag::TagsList& tags)
        {
            for (TagUpdater& updater: m_tagUpdaters)
                updater.setTags(tags);
        }

    private:
        std::deque<TagUpdater> m_tagUpdaters;
};



TagsModel::TagsModel(QObject* p):
    QStandardItemModel(p),
    m_selectionModel(nullptr),
    m_dbDataModel(nullptr)
{

}


TagsModel::~TagsModel()
{

}


void TagsModel::set(QItemSelectionModel* selectionModel)
{
    if (m_selectionModel != nullptr)
        m_selectionModel->disconnect(this);

    m_selectionModel = selectionModel;
    connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(refreshModel(QItemSelection, const QItemSelection &)));

    refreshModel();
}


void TagsModel::set(DBDataModel* dbDataModel)
{
    m_dbDataModel = dbDataModel;
}


void TagsModel::refreshModel()
{
    if (m_dbDataModel != nullptr && m_selectionModel != nullptr)
    {
        clearModel();

        std::vector<IPhotoInfo::Ptr> photos = getPhotosForSelection();
        TagGroupUpdater updater(photos);

        Tag::TagsList tags = updater.getTags();

        for (const auto& tag: tags)
        {
            Tag::Info info(tag);
            QStandardItem* name = new QStandardItem(info.name());
            QStandardItem* value = new QStandardItem(info.valuesString());

            const QList<QStandardItem *> items( { name, value });
            appendRow(items);
        }
    }
}


void TagsModel::clearModel()
{
    clear();
}


std::vector<IPhotoInfo::Ptr> TagsModel::getPhotosForSelection()
{
    std::vector<IPhotoInfo::Ptr> result;

    QItemSelection selection = m_selectionModel->selection();

    for (const QItemSelectionRange& range : selection)
    {
        QModelIndexList idxList = range.indexes();

        for (const QModelIndex& idx : idxList)
        {
            IPhotoInfo::Ptr photo = m_dbDataModel->getPhoto(idx);

            if (photo.get() != nullptr)
                result.push_back(photo);
        }
    }

    return result;
}


void TagsModel::refreshModel(const QItemSelection &, const QItemSelection &)
{
    refreshModel();
}
