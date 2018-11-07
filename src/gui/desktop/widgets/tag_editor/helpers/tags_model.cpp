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

#include <core/cross_thread_call.hpp>
#include <core/signal_postponer.hpp>
#include <database/idatabase.hpp>
#include "models/db_data_model.hpp"
#include "tags_operator.hpp"


using namespace std::chrono;
using namespace std::placeholders;

TagsModel::TagsModel(QObject* p):
    QAbstractItemModel(p),
    m_loadInProgress(false),
    m_selectionExtractor(),
    m_selectionModel(nullptr),
    m_dbDataModel(nullptr),
    m_tagsOperator(nullptr),
    m_database(nullptr)
{
}


TagsModel::~TagsModel()
{

}


void TagsModel::set(Database::IDatabase* database)
{
    m_database = database;
}


void TagsModel::set(QItemSelectionModel* selectionModel)
{
    m_selectionExtractor.set(selectionModel);

    if (m_selectionModel != nullptr)
        m_selectionModel->disconnect(this);

    m_selectionModel = selectionModel;
    connect(this, &TagsModel::dataChanged, this, &TagsModel::syncData);
    lazy_connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &TagsModel::refreshModel);
    lazy_connect(this, &TagsModel::emptyValueError, this, &TagsModel::refreshModel, 250ms, 500ms, Qt::QueuedConnection);   // refresh model on problems

    refreshModel();
}


void TagsModel::set(DBDataModel* dbDataModel)
{
    m_selectionExtractor.set(dbDataModel);
    m_dbDataModel = dbDataModel;
}


void TagsModel::set(ITagsOperator* tagsOperator)
{
    m_tagsOperator = tagsOperator;
}


Tag::TagsList TagsModel::getTags() const
{
    return m_tagsOperator->getTags();
}


void TagsModel::addTag(const TagNameInfo& info, const TagValue& value)
{
    m_tagsOperator->setTag(info, value);

    refreshModel();
}


bool TagsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    const QVector<int> touchedRoles = setDataInternal(index, value, role);
    const bool set = touchedRoles.empty() == false;

    if (set)
        emit dataChanged(index, index, touchedRoles);

    return set;
}


QVariant TagsModel::data(const QModelIndex& index, int role) const
{
    const int c = index.column();
    const int r = index.row();

    QVariant result;

    if (r < m_keys.size() && ( c == 0 || c == 1) )
    {
        const auto& vec = c == 0? m_keys: m_values;
        const auto& data  = vec[r];
        const auto it = data.constFind(role);

        result = it == data.end()? QVariant(): *it;
    }

    return result;
}


Qt::ItemFlags TagsModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = Qt::NoItemFlags;

    if (index.isValid() && index.column() < 2 && index.row() < m_keys.size())
    {
        flags =
          Qt::ItemIsEnabled     |
          Qt::ItemIsDropEnabled |
          Qt::ItemIsDragEnabled |
          Qt::ItemIsSelectable;

        if (index.column() == 1)
            flags |= Qt::ItemIsEditable;
    }

    return flags;
}



QModelIndex TagsModel::index(int row, int column, const QModelIndex& parent) const
{
    const QModelIndex result = parent.isValid()?
                                QModelIndex():
                                QAbstractItemModel::createIndex(row, column, nullptr);

    return result;
}


QModelIndex TagsModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}


int TagsModel::columnCount(const QModelIndex& parent) const
{
    const int c = parent.isValid()? 0 : 2;

    return c;
}


int TagsModel::rowCount(const QModelIndex& parent) const
{
    assert(m_keys.size() == m_values.size());

    const int r = parent.isValid()? 0 : m_keys.size();

    return r;
}


QVariant TagsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch(section)
        {
            case 0: return tr("Name");
            case 1: return tr("Value");
            default: return QVariant();
        }
    else
        return QVariant();
}


void TagsModel::refreshModel()
{
    if (m_dbDataModel != nullptr && m_selectionModel != nullptr && m_loadInProgress == false)
    {
        m_loadInProgress = true;

        clearModel();

        std::vector<Photo::Data> photos = m_selectionExtractor.getSelection();

        std::vector<Photo::Id> ids;
        for(const Photo::Data& photo: photos)
            ids.push_back(photo.id);

        auto target_fun = std::bind(&TagsModel::loadPhotos, this, _1);
        auto callback = make_cross_thread_function<const IPhotoInfo::List &>(this, target_fun);

        m_database->getPhotos(ids, callback);
    }
}


void TagsModel::clearModel()
{
    beginResetModel();
    m_keys.clear();
    m_values.clear();
    endResetModel();
}


void TagsModel::loadPhotos(const std::vector<IPhotoInfo::Ptr>& photos)
{
    m_tagsOperator->operateOn(photos);

    Tag::TagsList tags = getTags();

    assert(rowCount() == 0);

    if (tags.empty() == false)
    {
        const std::size_t tc = tags.size();
        QAbstractItemModel::beginInsertRows(QModelIndex(), 0, tc - 1);

        m_keys.resize(tc);
        m_values.resize(tc);

        int row = 0;

        for (const auto& tag: tags)
        {
            const Tag::Info info(tag);

            QModelIndex name = index(row, 0);
            QModelIndex value = index(row, 1);

            setData(name, info.displayName(), Qt::DisplayRole);

            const QVariant dispRole = info.value().get();
            const QVariant tagInfoRole = QVariant::fromValue(info.getTypeInfo());

            setData(value, dispRole, Qt::DisplayRole);
            setData(value, tagInfoRole, TagInfoRole);

            row++;
        }

        QAbstractItemModel::endInsertRows();
    }

    m_loadInProgress = false;
}


void TagsModel::syncData(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    const QItemSelection items(topLeft, bottomRight);
    const QModelIndexList itemsList(items.indexes());

    bool update_failed = false;

    for (const QModelIndex& itemIndex: itemsList)
    {
        // Do not react on changes in first column.
        // Such a change may be a reasult of new row appending.
        // Wait for the whole row to be filled.
        if (itemIndex.column() == 1)
        {
            const QVariant valueRaw = itemIndex.data();
            const TagValue value = TagValue::fromQVariant(valueRaw);

            const QVariant nameRaw = itemIndex.data(TagInfoRole);
            const TagNameInfo nameInfo = nameRaw.value<TagNameInfo>();

            if (value.rawValue().isEmpty())
                update_failed = true;
            else
                m_tagsOperator->insert(nameInfo, value);
        }
    }

    if (update_failed)
        emit emptyValueError();
}


QVector<int> TagsModel::setDataInternal(const QModelIndex& index, const QVariant& value, int role)
{
    const int c = index.column();
    const int r = index.row();

    QVector<int> touchedRoles;

    if (r < m_keys.size() && ( c == 0 || c == 1) )
    {
        touchedRoles.append(role);

        auto& vec = c == 0? m_keys: m_values;
        auto& data = vec[r];
        data[role] = value;

        // Item edited? Set DisplayRole too.
        if (role == Qt::EditRole)
        {
            data[Qt::DisplayRole] = value;
            touchedRoles.append(Qt::DisplayRole);
        }

        // Display role? Set EditRole too.
        if (role == Qt::DisplayRole)
        {
            data[Qt::EditRole] = value;
            touchedRoles.append(Qt::EditRole);
        }
    }

    return touchedRoles;
}
