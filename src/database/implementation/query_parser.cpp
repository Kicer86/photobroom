

#include <graphqlservice/GraphQLParse.h>

#include "query_parser.hpp"
#include "graphql/photo_broomSchema.h"
#include "graphql/QueryObject.h"

namespace graphql::database
{
    class Query
    {
        public:
            service::AwaitableObject<std::vector<std::shared_ptr<object::Photo>>> getPhotos(service::FieldParams&& params) const
            {

            }

            service::AwaitableObject<std::shared_ptr<object::Photo>> getPhoto(service::FieldParams&& params, response::IdType&& idArg) const
            {

            }

    };
}


void parseQuery(const QString& queryString)
{
    auto parsed = graphql::peg::parseString(queryString.toStdString());
    auto query = std::make_shared<graphql::database::Query>();
    auto service = std::make_shared<graphql::database::Operations>(std::move(query));
}
