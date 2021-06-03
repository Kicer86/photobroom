
#ifndef DB_THUMBNAILS_ACCESSOR_HPP_INCLUDED
#define DB_THUMBNAILS_ACCESSOR_HPP_INCLUDED

#include "ithumbnails_accessor.hpp"
#include "idatabase.hpp"
#include "database_export.h"


namespace Database
{
    class DATABASE_EXPORT ThumbnailsAccessor: public IThumbnailsAccessor
    {
        public:
            ThumbnailsAccessor(IDatabase &);

            QImage getThumbnail(const Photo::Id &) override;

        private:
            IDatabase& m_db;
    };
}

#endif // DB_THUMBNAILS_ACCESSOR_HPP_INCLUDED
