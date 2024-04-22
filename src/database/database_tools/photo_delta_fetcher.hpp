
#pragma once

#include <QObject>

#include <core/function_wrappers.hpp>
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
        safe_callback_ctrl m_callbackCtrl;
        Database::IDatabase& m_db;

        void storePhotoData(const std::vector<Photo::DataDelta> &);

    signals:
        void photoDataDeltaFetched(const std::vector<Photo::DataDelta> &) const;
};
