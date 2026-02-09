
#pragma once

#include <QObject>

#include <database/photo_data.hpp>
#include <database/idatabase.hpp>

#include "database_export.h"


class DATABASE_EXPORT PhotoDeltaFetcher: public QObject
{
        Q_OBJECT

    public:
        explicit PhotoDeltaFetcher(Database::IDatabase &);

        void fetchIds(const std::vector<Photo::Id> &, const std::set<Photo::Field> &);

    private:
        Database::IDatabase& m_db;

    signals:
        void photoDataDeltaFetched(const std::vector<Photo::DataDelta> &) const;
};
