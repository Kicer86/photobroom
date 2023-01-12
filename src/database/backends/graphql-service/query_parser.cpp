
#include <QDate>

#include "query_parser.hpp"


namespace
{
    class TagsAdapter
    {
    public:
        TagsAdapter(const Tag::TagsList& tags)
            : m_tags(tags)
        {
        }

        std::optional<graphql::response::Value> getDate(graphql::service::FieldParams&& params) const
        {
            auto it = m_tags.find(Tag::Date);
            return it == m_tags.end()?
                std::optional<graphql::response::Value>():
                graphql::response::Value(it->second.rawValue().toStdString());
        }

        std::optional<graphql::response::Value> getTime() const
        {
            auto it = m_tags.find(Tag::Time);
            return it == m_tags.end()?
                std::optional<graphql::response::Value>():
                graphql::response::Value(it->second.rawValue().toStdString());
        }

        std::optional<std::string> getEvent(graphql::service::FieldParams&& params) const
        {
            auto it = m_tags.find(Tag::Event);
            return it == m_tags.end()? std::optional<std::string>(): it->second.rawValue().toStdString();
        }

    private:
        const Tag::TagsList m_tags;
    };
}


namespace GraphQLParser
{
    PhotoDeltaAdapter::PhotoDeltaAdapter(const Photo::Id& id, Database::IBackend& backend)
        : m_id(id)
        , m_backend(backend)
    {

    }


    graphql::response::IdType PhotoDeltaAdapter::getId(graphql::service::FieldParams&& params) const
    {
        return std::to_string(m_id.value());
    }


    graphql::response::Value PhotoDeltaAdapter::getCreatedAt(graphql::service::FieldParams&& params) const
    {
        return {};
    }


    std::shared_ptr<graphql::database::object::Tags> PhotoDeltaAdapter::getTags(graphql::service::FieldParams&& params) const
    {
        auto data = m_backend.getPhotoDelta(m_id, {Photo::Field::Tags});
        auto tagsModel = std::make_shared<TagsAdapter>(data.get<Photo::Field::Tags>());
        return std::make_shared<graphql::database::object::Tags>(tagsModel);
    }


    graphql::response::Value PhotoDeltaAdapter::getPhash() const
    {
        return {};
    }
}
