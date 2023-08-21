
#ifndef PHOTOSDATACOMPLETER_HPP
#define PHOTOSDATACOMPLETER_HPP

#include <QAbstractListModel>
#include <QDateTime>

#include "database/idatabase.hpp"


/**
 * @brief class provides photos with extra information guessed from filename, content etc.
 */
class PhotosDataGuesser: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase REQUIRED)
    Q_PROPERTY(bool fetchInProgress READ isFetchInProgress NOTIFY fetchInProgressChanged)
    Q_PROPERTY(bool updateInProgress READ isUpdateInProgress NOTIFY updateInProgressChanged)

public:
    PhotosDataGuesser();

    void setDatabase(Database::IDatabase *);
    Database::IDatabase* database() const;

    bool isFetchInProgress() const;
    bool isUpdateInProgress() const;

    Q_INVOKABLE void performAnalysis();
    Q_INVOKABLE void apply(const QList<int> &);
    Q_INVOKABLE Photo::Id getId(int row) const;

    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void fetchInProgressChanged(bool) const;
    void updateInProgressChanged(bool) const;

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
    bool m_updating;

    void clear();
    void updateFetchStatus(bool);
    void updateUpdateStatus(bool);
    void process(Database::IBackend &);
    void processIds(Database::IBackend &, const std::vector<Photo::Id> &);
    void updatePhotos(Database::IBackend &, const std::vector<CollectedData> &);
    void photosFetched(const std::vector<Photo::Id> &);
    void photoDataFetched(const std::vector<CollectedData> &);
};

#endif
