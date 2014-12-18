
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

        TagNameInfo(const QString& name, const Type , const QString& searator = ";");
        TagNameInfo(const QString& name, int type, const QString& separator = ";");
        TagNameInfo(const TagNameInfo& other);

        operator QString() const;
        bool operator==(const TagNameInfo& other) const;
        bool operator<(const TagNameInfo& other) const;
        TagNameInfo& operator=(const TagNameInfo& other);

        const QString& getName() const;
        const QString& getSeparator() const;
        Type getType() const;

    private:
        QString name;
        Type type;
        QString separator;
};


class CORE_EXPORT TagValue
{
    public:
        TagValue();
        TagValue(const std::initializer_list<QString> &);
        ~TagValue();

        void setValue(const QString &);
        void setValues(const std::set<QString> &);
        void addValue(const QString &);

        const std::set<QString>& getValues() const;

        std::set<QString>::const_iterator begin() const;
        std::set<QString>::const_iterator end() const;

    private:
        std::set<QString> m_values;
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
            const TagValue& values() const;
            QString valuesString() const;

            bool setRawValues(const QString &);

        private:
            TagNameInfo m_name;
            TagValue m_values;
    };

}

#endif
