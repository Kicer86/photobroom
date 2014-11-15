
#ifndef TAG_HPP
#define TAG_HPP

#include <assert.h>

#include <vector>
#include <map>
#include <set>
#include <memory>

#include <QString>

#include "core_export.h"

struct CORE_EXPORT TagNameInfo
{
        enum Type
        {
            //indexed, as those values will be stored in db.
            Invalid = 0,
            Text    = 1,
            Date    = 2,
            Time    = 3,
        };

        TagNameInfo(const QString& name, const Type , char searator = ';');
        TagNameInfo(const QString& name, int type, char separator = ';');
        TagNameInfo(const TagNameInfo& other);

        operator QString() const;
        bool operator==(const TagNameInfo& other) const;
        bool operator<(const TagNameInfo& other) const;
        TagNameInfo& operator=(const TagNameInfo& other);

        const QString& getName() const;
        char getSeparator() const;
        Type getType() const;

    private:
        QString name;
        Type type;
        char separator;
};


struct CORE_EXPORT TagValueInfo
{
    QString m_value;

    TagValueInfo(const QString& v);

    operator QString() const;
    bool operator<(const TagValueInfo& other) const;
    const QString& value() const;
};

namespace Tag
{
    typedef std::set<TagValueInfo> ValuesSet;
    typedef std::map<TagNameInfo, ValuesSet> TagsList;

    struct CORE_EXPORT Info
    {
            Info(const Tag::TagsList::const_iterator &);
            Info(const std::pair<const TagNameInfo, Tag::ValuesSet> &data);

            Info& operator=(const std::pair<TagNameInfo, Tag::ValuesSet> &data);

            QString name() const;
            TagNameInfo getTypeInfo() const;
            const Tag::ValuesSet& values() const;
            QString valuesString() const;

        private:
            TagNameInfo m_name;
            Tag::ValuesSet m_values;
    };

}

#endif
