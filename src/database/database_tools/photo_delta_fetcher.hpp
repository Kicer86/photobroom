
#pragma once

#include <QObject>
#include <QFuture>

#include <database/photo_data.hpp>
#include <database/idatabase.hpp>

#include "database_export.h"


class DATABASE_EXPORT PhotoDeltaFetcher: public QObject
{
        Q_OBJECT

    public:
        explicit PhotoDeltaFetcher(Database::IDatabase &);
        ~PhotoDeltaFetcher();

        void fetchIds(const std::vector<Photo::Id> &, const std::set<Photo::Field> &);

    private:
        QFuture<std::vector<Photo::DataDelta>> m_dataFetchFuture;
        Database::IDatabase& m_db;

        void storePhotoData(const std::vector<Photo::DataDelta> &);

    signals:
        void photoDataDeltaFetched(const std::vector<Photo::DataDelta> &) const;
};
