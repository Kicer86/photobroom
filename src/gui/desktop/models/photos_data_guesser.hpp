
#ifndef PHOTOSDATACOMPLETER_HPP
#define PHOTOSDATACOMPLETER_HPP

#include <QAbstractListModel>
#include <QDateTime>

#include <database/idatabase.hpp>

#include "aheavy_list_model.hpp"


struct CollectedData
{
    Photo::DataDelta photoData;
    QDate date;
    QTime time;
};

/**
 * @brief class provides photos with extra information guessed from filename, content etc.
 */
class PhotosDataGuesser: public AHeavyListModel<CollectedData>
{
    Q_OBJECT
    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase REQUIRED)

public:
    PhotosDataGuesser();

    void setDatabase(Database::IDatabase *);
    Database::IDatabase* database() const;

    Q_INVOKABLE Photo::Id getId(int row) const;

    QVariant data(const QModelIndex & index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    Database::IDatabase* m_db;

    void loadData(QPromise<DataVector> &&) override;
    void applyRows(const QList<int> & , AHeavyListModel::ApplyToken ) override;
};

#endif
