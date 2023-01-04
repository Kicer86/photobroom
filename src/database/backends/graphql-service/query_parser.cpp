
#include "query_parser.hpp"


namespace GraphQLParser
{
    PhotoDeltaAdapter::PhotoDeltaAdapter(const Photo::DataDelta& data)
        : m_data(data)
    {

    }


    graphql::response::IdType PhotoDeltaAdapter::getId(graphql::service::FieldParams&& params) const
    {
        return std::to_string(m_data.getId().value());
    }


    graphql::response::Value PhotoDeltaAdapter::getCreatedAt(graphql::service::FieldParams&& params) const
    {
        return {};
    }


    std::shared_ptr<graphql::database::object::Tags> PhotoDeltaAdapter::getTags(graphql::service::FieldParams&& params) const
    {
        return {};
    }

}
