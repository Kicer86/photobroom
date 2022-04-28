
#ifndef DUPLICATES_MODEL_HPP_INCLUDED
#define DUPLICATES_MODEL_HPP_INCLUDED


#include <QAbstractListModel>
#include <database/idatabase.hpp>


class DuplicatesModel: public QAbstractListModel
{
public:
    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;

    void setDB(Database::IDatabase *);

private:
    QList<int> m_duplicates;
    Database::IDatabase* m_db;
};

#endif
