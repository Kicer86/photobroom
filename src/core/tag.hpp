
#ifndef TAG_HPP
#define TAG_HPP

#include <assert.h>

#include <deque>
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

        TagNameInfo(const QString& name, const Type);
        TagNameInfo(const QString& name, int type);
        TagNameInfo(const TagNameInfo& other);

        operator QString() const;
        bool operator==(const TagNameInfo& other) const;
        bool operator<(const TagNameInfo& other) const;
        TagNameInfo& operator=(const TagNameInfo& other);

        const QString& getName() const;
        Type getType() const;

    private:
        QString name;
        Type type;
};


class CORE_EXPORT TagValue
{
    public:
        typedef std::set<QString> List;

        TagValue();
        TagValue(const QString &);
        ~TagValue();

        void set(const QString &);
        void add(const QString &);

        const QString get() const;
        const List& getAll() const;

        bool operator==(const TagValue &) const;
        bool operator!=(const TagValue &) const;

    private:
        List m_values;
};


namespace Tag
{
    typedef std::map<TagNameInfo, TagValue> TagsList;

    struct CORE_EXPORT Info
    {
            Info(const TagsList::const_iterator &);
            Info(const std::pair<const TagNameInfo, TagValue> &data);

            Info& operator=(const std::pair<TagNameInfo, TagValue> &data);

            QString name() const;
            TagNameInfo getTypeInfo() const;
            const TagValue& value() const;

            void setValue(const QString &);

        private:
            TagNameInfo m_name;
            TagValue m_value;
    };

}

#endif
