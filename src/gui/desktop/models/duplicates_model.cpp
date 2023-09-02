
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
    const auto row = static_cast<std::size_t>(index.row());
    const auto& duplicates = internalData();

    if (role == DuplicatesRole && index.column() == 0 && row < duplicates.size())
        return QVariant::fromValue(duplicates[row]);
    else
        return {};
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


void DuplicatesModel::loadData(const std::stop_token& stopToken, StoppableTaskCallback<std::vector<std::vector<Photo::DataDelta>>> callback)
{
    m_db->exec([callback](Database::IBackend& backend)
    {
        const auto ids = backend.photoOperator().onPhotos(Database::FilterSimilarPhotos{}, Database::Actions::Sort(Database::Actions::Sort::By::PHash));

        std::vector<Photo::DataDelta> duplicatePhotos;
        duplicatePhotos.reserve(ids.size());

        for(const auto& id: ids)
            duplicatePhotos.push_back(backend.getPhotoDelta(id, {Photo::Field::PHash, Photo::Field::Path}));

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

        callback(grouped);
    },
    "Looking for photo duplicates"
    );
}


void DuplicatesModel::applyRows(const QList<int> &, ApplyToken)
{
    assert(!"Nothing to be done");
}
