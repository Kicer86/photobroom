
#ifndef PHOTOSDATACOMPLETER_HPP
#define PHOTOSDATACOMPLETER_HPP

#include <QAbstractListModel>

#include "database_export.h"
#include "database/idatabase.hpp"


/**
 * @brief class provides photos with extra information guessed from filename, content etc.
 */
class DATABASE_EXPORT PhotosDataGuesser: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase)

public:
    PhotosDataGuesser();

    void setDatabase(Database::IDatabase *);
    Database::IDatabase* database() const;

    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    struct CollectedData
    {
        Photo::DataDelta photoData;
        QString date;
    };

    Database::IDatabase* m_db;
    std::vector<CollectedData> m_photos;

    void proces(Database::IBackend &);
    void procesIds(Database::IBackend &, const std::vector<Photo::Id> &);
    void photosFetched(const std::vector<Photo::Id> &);
    void photoDataFetched(const std::vector<CollectedData> &);
};

#endif
