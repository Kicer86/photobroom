
#ifndef QUERY_RESPONSE_PARSER_HPP_INCLUDED
#define QUERY_RESPONSE_PARSER_HPP_INCLUDED

#include <QString>

#include "photo_data.hpp"

#include "database_export.h"

namespace Query
{
    /**
     * @brief return common fragments for graphQL queries
     *
     * @return set of common fragments. Currently fragments are:
     * - TagsFields - all tags for photo. Sample use:
     *   @code{.graphql}
     *   query {
     *       photos {
     *           id
     *           tags{
     *               ...TagsFields
     *           }
     *       }
     *   }
     *   @endcode
     */
    DATABASE_EXPORT QString commonFragments();
}

namespace ResponseParser
{
    DATABASE_EXPORT std::vector<Photo::DataDelta> parsePhotosQueryResponse(const QString &);
}

#endif
