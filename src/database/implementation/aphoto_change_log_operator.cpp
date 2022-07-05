
#include <core/containers_utils.hpp>
#include <core/base_tags.hpp>

#include "aphoto_change_log_operator.hpp"


namespace
{
    QString encodeTag(const TagValue& value)
    {
        const QString encodedValue = value.rawValue().toUtf8().toBase64();

        return encodedValue;
    }

    QString encodeTag(const Tag::Types& tag, const TagValue& value)
    {
        const QString encoded = QString("%1 %2")
                                    .arg(tag)
                                    .arg(encodeTag(value));

        return encoded;
    }

    QString encodeTag(const Tag::Types& tag, const TagValue& valueOld, const TagValue& valueNew)
    {
        const QString encoded = QString("%1 %2 %3")
                                    .arg(tag)
                                    .arg(encodeTag(valueOld))
                                    .arg(encodeTag(valueNew));

        return encoded;
    }

    QString decodeTag(const QString& encoded)
    {
        assert(encoded.split(" ").size() == 1);

        const QByteArray array = QByteArray::fromBase64(encoded.toUtf8());

        return array;
    }

    std::tuple<Tag::Types, TagValue> decodeTag2(const QString& encoded)
    {
        const QStringList items = encoded.split(" ");
        assert(items.size() == 2);

        const QString& tagInfoStr = items.front();
        const QString& tagValueStr = items.back();

        const Tag::Types tagType = static_cast<Tag::Types>(tagInfoStr.toInt());
        const QString decodedTagValue = decodeTag(tagValueStr);

        return std::make_tuple(tagType, decodedTagValue);
    }

    std::tuple<Tag::Types, TagValue, TagValue> decodeTag3(const QString& encoded)
    {
        const QStringList items = encoded.split(" ");
        assert(items.size() == 3);

        const QString& tagInfoStr = items[0];
        const QString& oldTagValueStr = items[1];
        const QString& newTagValueStr = items[2];

        const Tag::Types tagType = static_cast<Tag::Types>(tagInfoStr.toInt());
        const QString decodedOldTagValue = decodeTag(oldTagValueStr);
        const QString decodedNewTagValue = decodeTag(newTagValueStr);

        return std::make_tuple(tagType, decodedOldTagValue, decodedNewTagValue);
    }
}


namespace Database
{

    void APhotoChangeLogOperator::storeDifference(const Photo::Data& currentContent, const Photo::DataDelta& newContent)
    {
        assert(currentContent.id == newContent.getId());
        const Photo::Id& id = currentContent.id;

        if (newContent.has(Photo::Field::Tags))
        {
            const auto& oldTags = currentContent.tags;
            const auto& newTags = newContent.get<Photo::Field::Tags>();

            process(id, oldTags, newTags);
        }

        if (newContent.has(Photo::Field::GroupInfo))
        {
            const auto& oldGroupInfo = currentContent.groupInfo;
            const auto& newGroupInfo = newContent.get<Photo::Field::GroupInfo>();

            process(id, oldGroupInfo, newGroupInfo);
        }
    }


    void APhotoChangeLogOperator::groupCreated(const Group::Id& id, const Group::Type &, const Photo::Id& representative_id)
    {
        process(representative_id, GroupInfo(), GroupInfo(id, GroupInfo::Role::Representative));
    }


    void APhotoChangeLogOperator::groupDeleted(const Group::Id& id, const Photo::Id& representative, const std::vector<Photo::Id>& members)
    {
        process(representative, GroupInfo(id, GroupInfo::Role::Representative), GroupInfo());

        for(const Photo::Id& ph_id: members)
            process(ph_id, GroupInfo(id, GroupInfo::Role::Member), GroupInfo());
    }


    void APhotoChangeLogOperator::process(const Photo::Id& id, const Tag::TagsList& oldTags, const Tag::TagsList& newTags)
    {
        std::vector<std::pair<Tag::Types, TagValue>> tagsRemoved;
        std::vector<std::tuple<Tag::Types, TagValue, TagValue>> tagsChanged;
        std::vector<std::pair<Tag::Types, TagValue>> tagsAdded;

        compare(oldTags, newTags,
                std::back_inserter(tagsRemoved),
                std::back_inserter(tagsChanged),
                std::back_inserter(tagsAdded));

        for(const auto& d: tagsRemoved)
        {
            const QString data = encodeTag(d.first, d.second);
            append(id, Remove, Tags, data);
        }

        for(const auto& d: tagsChanged)
        {
            const QString data = encodeTag(std::get<0>(d), std::get<1>(d), std::get<2>(d));
            append(id, Modify, Tags, data);
        }

        for(const auto& d: tagsAdded)
        {
            const QString data = encodeTag(d.first, d.second);
            append(id, Add, Tags, data);
        }
    }


    void APhotoChangeLogOperator::process(const Photo::Id& id, const GroupInfo& oldGroupInfo, const GroupInfo& newGroupInfo)
    {
        if (oldGroupInfo.group_id.valid() && newGroupInfo.group_id.valid())   // both valid -> modification
        {
            const QString data = QString("%1 %2,%3 %4")
                                    .arg(oldGroupInfo.group_id.value())
                                    .arg(newGroupInfo.group_id.value())
                                    .arg(oldGroupInfo.role)
                                    .arg(newGroupInfo.role);

            append(id, Modify, Group, data);
        }
        else if (oldGroupInfo.group_id.valid() && !newGroupInfo.group_id)     // only old valid -> removal
        {
            const QString data = QString("%1,%2")
                                    .arg(oldGroupInfo.group_id.value())
                                    .arg(oldGroupInfo.role);

            append(id, Remove, Group, data);
        }
        else if (!oldGroupInfo.group_id && newGroupInfo.group_id.valid())    // only new valid -> addition
        {
            const QString data = QString("%1,%2")
                                    .arg(newGroupInfo.group_id.value())
                                    .arg(newGroupInfo.role);

            append(id, Add, Group, data);
        }
    }


    QString APhotoChangeLogOperator::format(int photoId, Operation operation, Field field, const QString& data)
    {
        const QString result = QString("photo id: %1. %2 %3. %4")
                                    .arg(photoId)
                                    .arg(fieldToStr(field))
                                    .arg(opToStr(operation))
                                    .arg(dataToStr(field, operation, data));

        return result;
    }


    QString APhotoChangeLogOperator::fieldToStr(Field field)
    {
        switch(field)
        {
            case Tags:  return "Tag";
            case Group: return "Group";
        }

        assert(!"unexpected");
        return "";
    }


    QString APhotoChangeLogOperator::opToStr(Operation op)
    {
        switch(op)
        {
            case Add:    return "added";
            case Modify: return "modified";
            case Remove: return "removed";
        }

        assert(!"unexpected");
        return "";
    }


    QString APhotoChangeLogOperator::dataToStr(APhotoChangeLogOperator::Field field,
                                               APhotoChangeLogOperator::Operation op,
                                               const QString& data)
    {
        QString result;

        switch(field)
        {
            case Tags:
                switch(op)
                {
                    case Add:
                    case Remove:
                    {
                        auto encoded = decodeTag2(data);
                        const Tag::Types tag_type(std::get<0>(encoded));
                        const TagValue& tag_value = std::get<1>(encoded);

                        result = QString("%1: %2")
                                    .arg(BaseTags::getName(tag_type))
                                    .arg(tag_value.rawValue());
                    }
                    break;

                    case Modify:
                    {
                        auto encoded = decodeTag3(data);
                        const Tag::Types tag_type(std::get<0>(encoded));
                        const TagValue& old_tag_value = std::get<1>(encoded);
                        const TagValue& new_tag_value = std::get<2>(encoded);

                        result = QString("%1: %2 -> %3")
                                    .arg(BaseTags::getName(tag_type))
                                    .arg(old_tag_value.rawValue())
                                    .arg(new_tag_value.rawValue());
                    }
                }
                break;

            case Group:
                switch(op)
                {
                    case Add:
                    case Remove:
                    {
                        const QStringList id_type = data.split(",");

                        result = QString("%1: %2")
                                    .arg(id_type[0])
                                    .arg(id_type[1]);
                    }
                    break;

                    case Modify:
                    {
                        const QStringList ids_types = data.split(",");
                        const QStringList ids = ids_types[0].split(" ");
                        const QStringList types = ids_types[1].split(" ");

                        result = QString("%1 -> %2, %3 -> %4")
                                    .arg(ids[0])
                                    .arg(ids[1])
                                    .arg(types[0])
                                    .arg(types[1]);
                    }
                }
                break;
        }

        return result;
    }

}
