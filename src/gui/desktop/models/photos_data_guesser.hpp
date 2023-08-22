
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
class PhotosDataGuesser: public AHeavyListModel<std::vector<CollectedData>>
{
    Q_OBJECT
    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase REQUIRED)

public:
    PhotosDataGuesser();

    void setDatabase(Database::IDatabase *);
    Database::IDatabase* database() const;

    Q_INVOKABLE void apply(const QList<int> &);
    Q_INVOKABLE Photo::Id getId(int row) const;

    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<CollectedData> m_photos;
    Database::IDatabase* m_db;

    void loadData(const std::stop_token &stopToken, StoppableTaskCallback<std::vector<CollectedData>>) override;
    void updateData(const std::vector<CollectedData> &) override;
    void clearData() override;

    void updatePhotos(Database::IBackend &, const std::vector<CollectedData> &);
};

#endif
