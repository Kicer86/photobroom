
#ifndef TYPES_HPP
#define TYPES_HPP

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

        TagNameInfo(const QString& n, const Type t, char s = ';'): name(n), type(t), separator(s)
        {

        }

        TagNameInfo(const QString& n, int t, char s = ';'): TagNameInfo(n, Type(t), s)
        {

        }

        TagNameInfo(const TagNameInfo& other): name(other.name), type(other.type), separator(other.separator)
        {

        }

        operator QString() const
        {
            return name;
        }

        bool operator==(const TagNameInfo& other) const
        {
            const bool result = name == other.name;

            assert(result == false || separator == other.separator);  //if result is true, then separators must be equal

            return result;
        }

        bool operator<(const TagNameInfo& other) const
        {
            const bool result = name < other.name;

            return result;
        }

        TagNameInfo& operator=(const TagNameInfo& other)
        {
            name = other.name;
            separator = other.separator;
            type = other.type;

            return *this;
        }

        const QString& getName() const
        {
            return name;
        }

        char getSeparator() const
        {
            return separator;
        }

        Type getType() const
        {
            return type;
        }

    private:
        QString name;
        Type type;
        char separator;
};


struct CORE_EXPORT TagValueInfo
{
    QString m_value;

    TagValueInfo(const QString& v): m_value(v) {}

    operator QString() const
    {
        return m_value;
    }

    bool operator<(const TagValueInfo& other) const
    {
        const bool result = m_value < other.m_value;

        return result;
    }

    const QString& value() const
    {
        return m_value;
    }
};


struct ITagData
{
        typedef std::set<TagValueInfo> ValuesSet;
        typedef std::map<TagNameInfo, ValuesSet> TagsList;

        struct TagInfo
        {
                TagInfo(const TagsList::const_iterator &it): m_name(it->first), m_values(it->second) {}
                TagInfo(const std::pair<TagNameInfo, ValuesSet> &data): m_name(data.first), m_values(data.second) {}

                TagInfo& operator=(const std::pair<TagNameInfo, ValuesSet> &data)
                {
                    m_name = data.first;
                    m_values = data.second;

                    return *this;
                }

                QString name() const
                {
                    return m_name;
                }

                TagNameInfo getTypeInfo() const
                {
                    return m_name;
                }

                const ValuesSet& values() const
                {
                    return m_values;
                }

                QString valuesString() const
                {
                    QString result;

                    for(const QString &str: m_values)
                    {
                        result += str + " ";                //TODO: temporary
                    }

                    return result.simplified();
                }

            private:
                TagNameInfo m_name;
                ValuesSet m_values;
        };

        virtual ~ITagData();

        //get list of tags
        virtual TagsList getTags() const = 0;

        //set tag and its values. Overvrite existing tags
        virtual void setTag(const TagNameInfo& name, const ValuesSet& values) = 0;
        virtual void setTag(const TagNameInfo& name, const TagValueInfo& value) = 0;
        virtual void setTags(const TagsList &) = 0;

        virtual void clear() = 0;

        virtual bool isValid() const = 0;
};

CORE_EXPORT std::ostream& operator<<(std::ostream &, const ITagData &);

class CORE_EXPORT TagDataBase: public ITagData
{
    public:
        TagDataBase();
        TagDataBase(const TagDataBase &) = delete;   //possible pure virtual calls when final object is not ready
        virtual ~TagDataBase();

        using ITagData::setTag;
        virtual void setTag(const TagNameInfo &, const TagValueInfo &) override;
        virtual void setTags(const TagsList &) override;

        TagDataBase& operator=(const TagDataBase &);
};

class CORE_EXPORT TagData: public TagDataBase
{
    public:
        TagData();
        TagData(const TagData &);
        virtual ~TagData();

        virtual TagsList getTags() const override;

        using TagDataBase::setTag;
        using TagDataBase::operator=;
        virtual void setTag(const TagNameInfo &, const ValuesSet &) override;
        virtual void clear() override;

        virtual bool isValid() const override;

    private:
        TagsList m_tags;
};

#endif
