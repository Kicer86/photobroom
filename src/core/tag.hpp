
#ifndef TAG_HPP
#define TAG_HPP

#include <assert.h>

#include <deque>
#include <map>
#include <set>
#include <memory>

#include <boost/any.hpp>

#include <QString>
#include <QVariant>

#include "core_export.h"


enum class TagType
{
    //indexed, as those values will be stored in db.
    Empty   = 0,
    String  = 1,
    Date    = 2,
    Time    = 3,
    List    = 4,
};


struct CORE_EXPORT TagNameInfo
{
        TagNameInfo();
        TagNameInfo(const QString& name, const TagType, const QString& displayName);
        TagNameInfo(const QString& name, const TagType);
        TagNameInfo(const TagNameInfo& other);

        operator QString() const;
        bool operator==(const TagNameInfo& other) const;
        bool operator<(const TagNameInfo& other) const;
        bool operator>(const TagNameInfo& other) const;
        TagNameInfo& operator=(const TagNameInfo& other);

        const QString& getName() const;
        const QString& getDisplayName() const;
        TagType getType() const;

    private:
        QString name;
        QString displayName;
        TagType type;

        QString dn(const QString &) const;
};


class CORE_EXPORT TagValue
{
    public:
        TagValue();
        TagValue(const TagValue &);
        TagValue(TagValue &&);

        TagValue(const QDate &);
        TagValue(const QTime &);
        TagValue(const std::deque<TagValue> &);
        TagValue(const QString &);

        static TagValue fromRaw(const QString &, const TagType &);
        //explicit TagValue(const QVariant &);

        ~TagValue();

        TagValue& operator=(const TagValue &);
        TagValue& operator=(TagValue &&);

        [[deprecated]] void set(const QVariant &);
        void set(const QDate &);
        void set(const QTime &);
        void set(const std::deque<TagValue> &);
        void set(const QString &);

        QVariant get() const;
        const QDate& getDate() const;
        const std::deque<TagValue>& getList() const;
        const QString& getString() const;
        const QTime& getTime() const;

        QDate& getDate();
        std::deque<TagValue>& getList();
        QString& getString();
        QTime& getTime();

        TagType type() const;
        QString formattedValue() const;

        bool operator==(const TagValue &) const;
        bool operator!=(const TagValue &) const;
        bool operator<(const TagValue &) const;

    private:
        TagType m_type;
        boost::any m_value;

        template<typename T>
        bool validate() const;

        template<typename T>
        const T* get() const
        {
            assert( validate<T>() );

            const T* v = boost::any_cast<T>(&m_value);

            return v;
        }

        template<typename T>
        T* get()
        {
            assert( validate<T>() );

            T* v = boost::any_cast<T>(&m_value);

            return v;
        }

        QString string() const;
        TagValue& fromString(const QString &, const TagType &);
};

template<TagType T>
struct TagValueTraits {};

template<>
struct TagValueTraits<TagType::Date>
{
    typedef QDate StorageType;
};

template<>
struct TagValueTraits<TagType::List>
{
    typedef std::deque<TagValue> StorageType;
};

template<>
struct TagValueTraits<TagType::String>
{
    typedef QString StorageType;
};

template<>
struct TagValueTraits<TagType::Time>
{
    typedef QTime StorageType;
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
    };

}

Q_DECLARE_METATYPE(TagNameInfo)

#endif
