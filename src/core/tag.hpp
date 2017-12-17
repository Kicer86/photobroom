
#ifndef TAG_HPP
#define TAG_HPP

#include <assert.h>

#include <any>
#include <deque>
#include <map>
#include <set>
#include <memory>


#include <QString>
#include <QVariant>

#include "core_export.h"

enum BaseTagsList
{
    //indexed, as those values will be stored in db and should not change without a reason.
    Invalid = 0,
    Event   = 1,
    Place   = 2,
    Date    = 3,
    Time    = 4,
    People  = 5,
};


struct CORE_EXPORT TagNameInfo
{
        enum class Type
        {
            Invalid,
            String,
            Date,
            Time,
        };

        TagNameInfo();
        explicit TagNameInfo(const BaseTagsList &);
        TagNameInfo(const TagNameInfo& other);

        //operator QString() const;
        bool operator==(const TagNameInfo& other) const;
        bool operator<(const TagNameInfo& other) const;
        bool operator>(const TagNameInfo& other) const;
        TagNameInfo& operator=(const TagNameInfo& other);

        QString getName() const;
        QString getDisplayName() const;
        Type getType() const;
        BaseTagsList getTag() const;
        bool isMultiValue() const;

    private:
        BaseTagsList m_tag;
};


class CORE_EXPORT TagValue
{
    public:
        enum class Type
        {
            Empty,
            String,
            Date,
            Time,
            List,
        };

        TagValue();
        TagValue(const TagValue &);
        TagValue(TagValue &&);

        TagValue(const QDate &);
        TagValue(const QTime &);
        TagValue(const std::deque<TagValue> &);
        TagValue(const QString &);

        static TagValue fromRaw(const QString &, const TagNameInfo::Type &);    // tag's value as stored in db
        static TagValue fromQVariant(const QVariant &);

        ~TagValue();

        TagValue& operator=(const TagValue &);
        TagValue& operator=(TagValue &&);

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

        Type type() const;
        QString rawValue() const;                                               // tag's value as stored in db

        bool operator==(const TagValue &) const;
        bool operator!=(const TagValue &) const;
        bool operator<(const TagValue &) const;

    private:
        Type m_type;
        std::any m_value;

        template<typename T>
        bool validate() const;

        template<typename T>
        const T* get() const
        {
            assert( validate<T>() );

            const T* v = std::any_cast<T>(&m_value);

            return v;
        }

        template<typename T>
        T* get()
        {
            assert( validate<T>() );

            T* v = std::any_cast<T>(&m_value);

            return v;
        }

        QString string() const;
        TagValue& fromString(const QString &, const TagNameInfo::Type &);
};

template<TagValue::Type T>
struct TagValueTraits {};

template<>
struct TagValueTraits<TagValue::Type::Date>
{
    typedef QDate StorageType;
};

template<>
struct TagValueTraits<TagValue::Type::List>
{
    typedef std::deque<TagValue> StorageType;
};

template<>
struct TagValueTraits<TagValue::Type::String>
{
    typedef QString StorageType;
};

template<>
struct TagValueTraits<TagValue::Type::Time>
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
            Info(const TagNameInfo &, const TagValue &);

            Info& operator=(const std::pair<TagNameInfo, TagValue> &data);

            QString name() const;
            QString displayName() const;

            const TagNameInfo& getTypeInfo() const;
            const TagValue& value() const;

            void setValue(const TagValue &);

        private:
            TagNameInfo m_name;
            TagValue m_value;
    };

}

Q_DECLARE_METATYPE(TagNameInfo)

#endif
