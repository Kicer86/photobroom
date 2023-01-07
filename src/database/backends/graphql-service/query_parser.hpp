
#ifndef QUERY_PARSER_HPP_INCLUDED
#define QUERY_PARSER_HPP_INCLUDED

#include <string>
#include <graphqlservice/GraphQLParse.h>
#include <graphqlservice/JSONResponse.h>

#include <database/photo_data.hpp>
#include <database/ibackend.hpp>

#include "graphql/photo_broomSchema.h"
#include "graphql/QueryObject.h"
#include "graphql/PhotoObject.h"
#include "graphql/TagsObject.h"


namespace GraphQLParser
{
    class PhotoDeltaAdapter
    {
    public:
        explicit PhotoDeltaAdapter(const Photo::Id &, Database::IBackend &);

        graphql::response::IdType getId(graphql::service::FieldParams&& params) const;
        graphql::response::Value getCreatedAt(graphql::service::FieldParams&& params) const;
        std::shared_ptr<graphql::database::object::Tags> getTags(graphql::service::FieldParams&& params) const;

    private:
        const Photo::Id m_id;
        Database::IBackend& m_backend;
    };

    template<typename TQuery>
    std::string parseQuery(const std::string& queryString, std::shared_ptr<TQuery> query)
    {
        auto parsed = graphql::peg::parseString(queryString);
        auto service = std::make_shared<graphql::database::Operations>(std::move(query));

        auto response = service->resolve({ parsed });
        const auto json = graphql::response::toJSON(response.get());

        return json;
    }
}

#endif
