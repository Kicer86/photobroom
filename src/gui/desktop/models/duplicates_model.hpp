
#ifndef DUPLICATES_MODEL_HPP_INCLUDED
#define DUPLICATES_MODEL_HPP_INCLUDED


#include <QAbstractListModel>
#include <database/idatabase.hpp>


class DuplicatesModel: public QAbstractListModel
{
public:
    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;
    QHash<int, QByteArray> roleNames() const override;

    void setDB(Database::IDatabase *);

private:
    std::vector<std::vector<Photo::DataDelta>> m_duplicates;
    Database::IDatabase* m_db = nullptr;
    bool m_loaded = false;

    void compileDuplicates(const std::vector<Photo::DataDelta> &);
};

#endif
