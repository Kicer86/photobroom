
#ifndef TAG_HPP
#define TAG_HPP

#include <assert.h>

#include <deque>
#include <map>
#include <set>
#include <memory>

#include <QString>
#include <QVariant>

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

        TagNameInfo();
        TagNameInfo(const QString& name, const Type, const QString& displayName);
        TagNameInfo(const QString& name, const Type);
        TagNameInfo(const QString& name, int type);
        TagNameInfo(const TagNameInfo& other);

        operator QString() const;
        bool operator==(const TagNameInfo& other) const;
        bool operator<(const TagNameInfo& other) const;
        TagNameInfo& operator=(const TagNameInfo& other);

        const QString& getName() const;
        const QString& getDisplayName() const;
        Type getType() const;

    private:
        QString name;
        QString displayName;
        Type type;

        QString dn(const QString &) const;
};


class CORE_EXPORT TagValue
{
    public:
        TagValue();
        TagValue(const QVariant &);
        ~TagValue();

        void set(const QVariant &);

        const QVariant& get() const;

        bool operator==(const TagValue &) const;
        bool operator!=(const TagValue &) const;

    private:
        QVariant m_value;
};


namespace Tag
{
    typedef std::map<TagNameInfo, TagValue> TagsList;

    struct CORE_EXPORT Info
    {
            Info(const TagsList::const_iterator &);
            Info(const std::pair<const TagNameInfo, TagValue> &data);
            Info(const TagNameInfo &, const QVariant &);

            Info& operator=(const std::pair<TagNameInfo, TagValue> &data);

            const QString& name() const;
            const QString& displayName() const;

            const TagNameInfo& getTypeInfo() const;
            const TagValue& value() const;

            void setValue(const QVariant &);

        private:
            TagNameInfo m_name;
            TagValue m_value;
            
            void validate() const;
    };

}

#endif
