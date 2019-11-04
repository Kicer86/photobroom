
#ifndef TAG_HPP
#define TAG_HPP

#include <cassert>

#include <any>
#include <vector>
#include <map>
#include <set>
#include <memory>

#include <QString>
#include <QVariant>

#include "core_export.h"

/**
 * \brief List of tag types
 */
enum BaseTagsList
{
    // indexed, as those values will be stored in db and should not change without a reason.
    Invalid = 0,
    Event   = 1,
    Place   = 2,
    Date    = 3,
    Time    = 4,
    _People [[deprecated]] = 5,        // not valid anymore
};


struct CORE_EXPORT TagNameInfo
{
        /**
         * \brief types for holding particular tag type
         */
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

    private:
        BaseTagsList m_tag;
};



template<typename T>
struct TagValueTraits {};


class CORE_EXPORT TagValue
{
    public:
        enum class Type
        {
            Empty,
            String,
            Date,
            Time,
        };

        TagValue();
        TagValue(const TagValue &);
        TagValue(TagValue &&);

        template<typename T>
        TagValue(const T& value)
        {
            set(value);
        }

        static TagValue fromRaw(const QString &, const TagNameInfo::Type &);    // tag's value as stored in db
        static TagValue fromQVariant(const QVariant &);

        ~TagValue();

        TagValue& operator=(const TagValue &);
        TagValue& operator=(TagValue &&);

        template<typename T>
        void set(const T& value)
        {
            static_assert(sizeof(typename TagValueTraits<T>::StorageType) > 0, "Unexpected type");

            m_value = value;
            m_type = TagValueTraits<T>::type;
        }

        QVariant get() const;
        const QDate& getDate() const;
        const QString& getString() const;
        const QTime& getTime() const;

        template<typename T>
        const T& get() const
        {
            static_assert(sizeof(typename TagValueTraits<T>::StorageType) > 0, "Unexpected type");
            assert( validate<T>() );

            const T* v = std::any_cast<T>(&m_value);

            return *v;
        }

        Type type() const;
        QString rawValue() const;                                               // tag's value as stored in db

        bool operator==(const TagValue &) const;
        bool operator!=(const TagValue &) const;
        bool operator<(const TagValue &) const;

    private:
        Type m_type;
        std::any m_value;

        template<typename T>
        bool validate() const
        {
            return m_type == TagValueTraits<T>::type && m_value.has_value() && m_value.type() == typeid(T);
        }

        QString string() const;
        TagValue& fromString(const QString &, const TagNameInfo::Type &);
};


template<>
struct TagValueTraits<QString>
{
    typedef QString StorageType;
    constexpr static auto type = TagValue::Type::String;
};

template<>
struct TagValueTraits<QDate>
{
    typedef QDate StorageType;
    constexpr static auto type = TagValue::Type::Date;
};


template<>
struct TagValueTraits<QTime>
{
    typedef QTime StorageType;
    constexpr static auto type = TagValue::Type::Time;
};


namespace Tag
{
    typedef std::map<TagNameInfo, TagValue> TagsList;

    struct CORE_EXPORT Info
    {
            Info(const std::pair<const TagNameInfo, TagValue> &data);

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
