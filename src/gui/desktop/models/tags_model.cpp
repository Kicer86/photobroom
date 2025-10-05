/*
 * Photo Broom - photos management tool.
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

#include "tags_model.hpp"

#include <QItemSelectionModel>
#include <QDateTime>

#include <core/function_wrappers.hpp>
#include <core/base_tags.hpp>
#include <core/signal_postponer.hpp>
#include <database/idatabase.hpp>

#include "gui/desktop/utils/photo_delta_fetcher_binding.hpp"


using namespace std::chrono;
using namespace std::placeholders;


TagsModel::TagsModel(QObject* p):
    QAbstractItemModel(p),
    m_fetcher(*this, &TagsModel::loadPhotos),
    m_database(nullptr)
{
    connect(this, &TagsModel::dataChanged, this, &TagsModel::syncData);
}


TagsModel::~TagsModel()
{

}


void TagsModel::set(Database::IDatabase* database)
{
    m_database = database;
    m_tagsOperator.setDb(database);

    m_fetcher.setDatabase(m_database);
}


void TagsModel::setPhotos(const std::vector<Photo::Id>& photos)
{
    if (m_database)
    {
        setBusy(true);
        m_fetcher.fetchIds(photos, {Photo::Field::Tags});
    }
}


Tag::TagsList TagsModel::getTags() const
{
    return m_tagsOperator.getTags();
}


Database::IDatabase* TagsModel::getDatabase() const
{
    return m_database;
}


bool TagsModel::busy() const
{
    return m_busy;
}


bool TagsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    const QVariant fixedValue = correctInput(index, value);
    const QVector<int> touchedRoles = setDataInternal(index, fixedValue, role);
    const bool set = touchedRoles.empty() == false;

    if (set)
        emit dataChanged(index, index, touchedRoles);

    return set;
}


bool TagsModel::setItemData(const QModelIndex& index, const QMap<int, QVariant>& roles)
{
    const unsigned r = static_cast<unsigned>(index.row());
    const int c = index.column();

    assert(r < m_keys.size());
    assert(r < m_values.size());
    assert(c == 0 || c == 1);

    auto& vec = c == 0? m_keys: m_values;
    auto& data = vec[r];

    data = roles;

    emit dataChanged(index, index);

    return true;
}


bool TagsModel::insertRows(int row, int count, const QModelIndex& parent)
{
    assert(row == static_cast<int>(m_keys.size()));
    assert(row == static_cast<int>(m_values.size()));
    assert(parent.isValid() == false);

    const std::size_t s = m_keys.size();
    const std::size_t newSize = s + count;

    beginInsertRows(parent, row, row + count -1);

    m_keys.resize(newSize);
    m_values.resize(newSize);

    endInsertRows();

    return true;
}


QHash<int, QByteArray> TagsModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();

    names.insert(TagTypeRole, QByteArray("tagType"));

    return names;
}


QVariant TagsModel::data(const QModelIndex& index, int role) const
{
    const int c = index.column();
    const int r = index.row();

    QVariant result;

    if (r < static_cast<int>(m_keys.size()) && ( c == 0 || c == 1) )
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

    if (index.isValid() && index.column() < 2 && index.row() < static_cast<int>(m_keys.size()))
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


QModelIndex TagsModel::parent(const QModelIndex &) const
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

    const int r = parent.isValid()? 0 : static_cast<int>(m_keys.size());

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


void TagsModel::clearModel()
{
    beginResetModel();
    m_keys.clear();
    m_values.clear();
    endResetModel();
}


void TagsModel::loadPhotos(const std::vector<Photo::DataDelta>& deltas)
{
    clearModel();

    m_tagsOperator.operateOn(Photo::EDV<Photo::ExplicitDelta<Photo::Field::Tags>>(deltas));

    const Tag::TagsList photo_tags = getTags();
    const std::vector<Tag::Types> all_tags = BaseTags::getAll();

    std::vector<std::pair<Tag::Types, TagValue>> tags(photo_tags.cbegin(), photo_tags.cend());

    // to the list of photo's tags append tags with empty values
    for (const Tag::Types& base_tag: all_tags)
    {
        auto f = std::find_if(photo_tags.cbegin(), photo_tags.cend(),
                              [base_tag](const Tag::TagsList::value_type& tag_data)
        {
            return tag_data.first == base_tag;
        });

        if (f == photo_tags.cend())
            tags.emplace_back(base_tag, TagValue());
    }

    assert(rowCount() == 0);
    assert(tags.empty() == false);

    const auto tc = tags.size();
    QAbstractItemModel::beginInsertRows(QModelIndex(), 0, static_cast<int>(tc) - 1);

    m_keys.resize(tc);
    m_values.resize(tc);

    int row = 0;

    for (const auto& tag: tags)
    {
        const Tag::Types& tag_type = tag.first;
        const TagValue tag_value(tag.second);

        QModelIndex name = index(row, 0);
        QModelIndex value = index(row, 1);

        setDataInternal(name, BaseTags::getTr(tag_type), Qt::DisplayRole);

        const QVariant dispRole = tag_value.get();
        const QVariant tagTypeRole = QVariant::fromValue(tag_type);

        setDataInternal(value, dispRole, Qt::DisplayRole);
        setDataInternal(value, tagTypeRole, TagTypeRole);

        row++;
    }

    QAbstractItemModel::endInsertRows();

    setBusy(false);
}


void TagsModel::syncData(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    const QItemSelection items(topLeft, bottomRight);
    const QModelIndexList itemsList(items.indexes());

    for (const QModelIndex& itemIndex: itemsList)
    {
        // Do not react on changes in first column.
        // Such a change may be a result of new row appending.
        // Wait for the whole row to be filled.
        if (itemIndex.column() == 1)
        {
            const QVariant valueRaw = itemIndex.data();
            const TagValue value = valueRaw.isNull() || valueRaw == QString()?
                    TagValue():
                    TagValue::fromQVariant(valueRaw);

            const QVariant typeRaw = itemIndex.data(TagTypeRole);
            const Tag::Types type = typeRaw.value<Tag::Types>();

            m_tagsOperator.insert(type, value);
        }
    }
}


QVector<int> TagsModel::setDataInternal(const QModelIndex& index, const QVariant& value, int role)
{
    const int c = index.column();
    const int r = index.row();

    QVector<int> touchedRoles;

    if (r < static_cast<int>(m_keys.size()) && (c == 0 || c == 1) )
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


QVariant TagsModel::correctInput(const QModelIndex& idx, const QVariant& value) const
{
    // qml views are unable to distinguish between QTime and QDate as QDateTime is being used
    // fix that
    QVariant result = value;

    if (idx.column() == 1)      // values
    {
        const int type = value.typeId();
        if (type == QMetaType::Type::QDateTime)
        {
            const auto& roles = m_values[static_cast<unsigned>(idx.row())];
            auto it = roles.find(TagTypeRole);

            if (it != roles.end())
            {
                const auto tagType = it->value<Tag::Types>();

                if (tagType == Tag::Types::Date)
                    result = value.toDate();
                else if (tagType == Tag::Types::Time)
                    result = value.toTime();
                else
                    assert(!"not expected situation");
            }
        }
    }

    return result;
}


void TagsModel::setBusy(bool b)
{
    m_busy = b;

    emit busyChanged(b);
}
