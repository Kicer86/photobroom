
#ifndef DB_THUMBNAILS_ACCESSOR_HPP_INCLUDED
#define DB_THUMBNAILS_ACCESSOR_HPP_INCLUDED

#include <core/ithumbnails_generator.hpp>

#include "ithumbnails_accessor.hpp"
#include "idatabase.hpp"
#include "database_export.h"


namespace Database
{
    class DATABASE_EXPORT ThumbnailsAccessor: public IThumbnailsAccessor
    {
        public:
            ThumbnailsAccessor(IDatabase &, IThumbnailsGenerator &);

            QImage getThumbnail(const Photo::Id &) override;

        private:
            IDatabase& m_db;
            IThumbnailsGenerator& m_generator;
    };
}

#endif // DB_THUMBNAILS_ACCESSOR_HPP_INCLUDED
