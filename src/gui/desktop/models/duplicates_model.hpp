
#ifndef DUPLICATES_MODEL_HPP_INCLUDED
#define DUPLICATES_MODEL_HPP_INCLUDED

#include <database/idatabase.hpp>
#include "aheavy_list_model.hpp"


class DuplicatesModel: public AHeavyListModel<std::vector<Photo::DataDelta>>
{
    Q_OBJECT
    Q_PROPERTY(Database::IDatabase* database READ db WRITE setDB NOTIFY dbChanged)

public:
    DuplicatesModel();
    ~DuplicatesModel() override;

    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setDB(Database::IDatabase *);
    Database::IDatabase* db() const;

private:
    Database::IDatabase* m_db = nullptr;

    void loadData(const std::stop_token& stopToken, StoppableTaskCallback<std::vector<std::vector<Photo::DataDelta>>>) override;
    void applyRows(const QList<int> & , AHeavyListModel::ApplyToken ) override;

signals:
    void dbChanged() const;
};

#endif
