
#include "tag.hpp"

#include <iostream>
#include <algorithm>

#include "algo.hpp"

ITagData::~ITagData()
{

}


std::ostream& operator<<(std::ostream& stream, const ITagData &tagData)
{
    for (std::pair<TagNameInfo, Tag::ValuesSet> tags: tagData.getTags())
    {
        stream << tags.first.getName().toStdString() << ": ";

        Tag::ValuesSet::const_iterator valuesIt = tags.second.cbegin();
        Tag::ValuesSet::const_iterator valuesEnd = tags.second.cend();

        while (valuesIt != valuesEnd)
        {
            stream << valuesIt->value().toStdString();

            ++valuesIt;

            if (valuesIt != valuesEnd)
                stream << tags.first.getSeparator() << " ";
        }

        stream << std::endl;
    }

    return stream;
}
