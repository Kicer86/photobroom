
#ifndef PHOTOSDATACOMPLETER_HPP
#define PHOTOSDATACOMPLETER_HPP

#include <QAbstractListModel>
#include <QDateTime>

#include "database_export.h"
#include "database/idatabase.hpp"


/**
 * @brief class provides photos with extra information guessed from filename, content etc.
 */
class DATABASE_EXPORT PhotosDataGuesser: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase)
    Q_PROPERTY(bool fetchInProgress READ isFetchInProgress NOTIFY fetchInProgressChanged)

public:
    PhotosDataGuesser();

    void setDatabase(Database::IDatabase *);
    Database::IDatabase* database() const;

    bool isFetchInProgress() const;

    Q_INVOKABLE void performAnalysis();
    Q_INVOKABLE void applyBut(const QList<int> &);

    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void fetchInProgressChanged(bool) const;

private:
    struct CollectedData
    {
        Photo::DataDelta photoData;
        QDate date;
        QTime time;
    };

    Database::IDatabase* m_db;
    std::vector<CollectedData> m_photos;
    bool m_fetching;

    void clear();
    void updateFetchStatus(bool);
    void proces(Database::IBackend &);
    void procesIds(Database::IBackend &, const std::vector<Photo::Id> &);
    void updatePhotos(Database::IBackend &, const std::vector<CollectedData> &);
    void photosFetched(const std::vector<Photo::Id> &);
    void photoDataFetched(const std::vector<CollectedData> &);
};

#endif
