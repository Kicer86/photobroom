
#ifndef QUERY_RESPONSE_PARSER_HPP_INCLUDED
#define QUERY_RESPONSE_PARSER_HPP_INCLUDED

#include <QString>

#include "photo_data.hpp"

#include "database_export.h"

namespace ResponseParser
{
    DATABASE_EXPORT std::vector<Photo::DataDelta> parsePhotosQueryResponse(const QString &);
}

#endif
