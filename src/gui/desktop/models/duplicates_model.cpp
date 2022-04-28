
#include "duplicates_model.hpp"


QVariant DuplicatesModel::data(const QModelIndex& index, int role ) const
{
    return {};
}


int DuplicatesModel::rowCount(const QModelIndex& parent ) const
{
    return m_duplicates.size();
}


void DuplicatesModel::setDB(Database::IDatabase* db)
{
    m_db = db;
}
