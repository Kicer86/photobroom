
#include <core/qmodel_utils.hpp>
#include <database/iphoto_operator.hpp>
#include <database/photo_utils.hpp>

#include "duplicates_model.hpp"


enum Roles
{
    DuplicatesRole = Qt::UserRole + 1,
};


ENUM_ROLES_SETUP(Roles);


DuplicatesModel::DuplicatesModel()
{

}


DuplicatesModel::~DuplicatesModel()
{

}




QVariant DuplicatesModel::data(const QModelIndex& index, int role) const
{
    const auto row = index.row();

    if (role == DuplicatesRole && index.column() == 0 && row < m_duplicates.size())
        return QVariant::fromValue(m_duplicates[row]);
    else
        return {};
}


int DuplicatesModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0: static_cast<int>(m_duplicates.size());
}


QHash<int, QByteArray> DuplicatesModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    const auto extra = parseRoles<Roles>();
    const QHash<int, QByteArray> extraRoles(extra.begin(), extra.end());
    roles.insert(extraRoles);

    return roles;
}


void DuplicatesModel::setDB(Database::IDatabase* db)
{
    assert(state() == State::Idle || state() == State::Loaded);

    m_db = db;
    clear();

    emit dbChanged();
}



Database::IDatabase* DuplicatesModel::db() const
{
    return m_db;
}


void DuplicatesModel::loadData(const std::stop_token& stopToken, StoppableTaskCallback<std::vector<Photo::DataDelta>> callback)
{
    m_db->exec([callback](Database::IBackend& backend)
    {
        const auto ids = backend.photoOperator().onPhotos(Database::FilterSimilarPhotos{}, Database::Actions::Sort(Database::Actions::Sort::By::PHash));

        std::vector<Photo::DataDelta> data;
        data.reserve(ids.size());

        for(const auto& id: ids)
            data.push_back(backend.getPhotoDelta(id, {Photo::Field::PHash, Photo::Field::Path}));

        callback(data);
    },
    "Looking for photo duplicates"
    );
}


void DuplicatesModel::updateData(const std::vector<Photo::DataDelta>& duplicatePhotos)
{
    std::vector<std::vector<Photo::DataDelta>> grouped;

    for(auto it = duplicatePhotos.begin(); it != duplicatePhotos.end();)
    {
        const auto currentPHash = it->get<Photo::Field::PHash>();

        auto nextIt = std::find_if_not(it, duplicatePhotos.end(), [&currentPHash](const Photo::DataDelta& data) {
            return data.get<Photo::Field::PHash>() == currentPHash;
        });

        grouped.push_back(std::vector(it, nextIt));

        it = nextIt;
    }

    beginInsertRows({}, 0, grouped.size() - 1);
    m_duplicates.swap(grouped);
    endInsertRows();
}


void DuplicatesModel::clearData()
{
    beginResetModel();
    m_duplicates.clear();
    endResetModel();
}
