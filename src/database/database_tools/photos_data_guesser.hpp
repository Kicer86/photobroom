
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
public:
    PhotosDataGuesser(Database::IDatabase &);

    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;

private:
    Database::IDatabase& m_db;
    std::vector<Photo::DataDelta> m_photos;

    void proces(Database::IBackend &);
    void procesIds(Database::IBackend &, const std::vector<Photo::Id> &);
    void photosFetched(const std::vector<Photo::Id> &);
    void photoDataFetched(const std::vector<Photo::DataDelta> &);
};

#endif
