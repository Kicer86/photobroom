
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
enum TagTypes
{
    // indexed, as those values will be stored in db and should not change without a reason.
    Invalid = 0,
    Event   = 1,
    Place   = 2,
    Date    = 3,
    Time    = 4,
    _People [[deprecated]] = 5,        // not valid anymore
    Rating  = 6,
    Category = 7,
};

namespace Tag
{
    /**
     * @brief List of possible tag value types
     */
    enum class ValueType
    {
        Empty,
        String,
        Date,
        Time,
        Int,
        Uint64,
    };
}


class CORE_EXPORT TagTypeInfo
{
    public:
        TagTypeInfo();
        explicit TagTypeInfo(const TagTypes &);
        TagTypeInfo(const TagTypeInfo& other);

        //operator QString() const;
        bool operator==(const TagTypeInfo& other) const;
        bool operator<(const TagTypeInfo& other) const;
        bool operator>(const TagTypeInfo& other) const;
        TagTypeInfo& operator=(const TagTypeInfo& other);

        QString getName() const;
        QString getDisplayName() const;
        TagTypes getTag() const;

    private:
        TagTypes m_tag;
};



template<typename T>
struct TagValueTraits {};


class CORE_EXPORT TagValue
{
    public:
        TagValue();
        TagValue(const TagValue &);
        TagValue(TagValue &&);

        template<typename T>
        TagValue(const T& value)
        {
            set(value);
        }

        static TagValue fromRaw(const QString &, const Tag::ValueType &);    // tag's value as stored in db
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

        Tag::ValueType type() const;
        QString rawValue() const;                                               // tag's value as stored in db

        bool operator==(const TagValue &) const;
        bool operator!=(const TagValue &) const;
        bool operator<(const TagValue &) const;

    private:
        Tag::ValueType m_type;
        std::any m_value;

        template<typename T>
        bool validate() const
        {
            return m_type == TagValueTraits<T>::type && m_value.has_value() && m_value.type() == typeid(T);
        }

        QString string() const;
        TagValue& fromString(const QString &, const Tag::ValueType &);
};


template<>
struct TagValueTraits<QString>
{
    typedef QString StorageType;
    constexpr static auto type = Tag::ValueType::String;
};

template<>
struct TagValueTraits<QDate>
{
    typedef QDate StorageType;
    constexpr static auto type = Tag::ValueType::Date;
};


template<>
struct TagValueTraits<QTime>
{
    typedef QTime StorageType;
    constexpr static auto type = Tag::ValueType::Time;
};


template<>
struct TagValueTraits<int>
{
    typedef int StorageType;
    constexpr static auto type = Tag::ValueType::Int;
};


template<>
struct TagValueTraits<quint64>
{
    typedef quint64 StorageType;
    constexpr static auto type = Tag::ValueType::Uint64;
};


namespace Tag
{
    typedef std::map<TagTypeInfo, TagValue> TagsList;

    struct CORE_EXPORT Info
    {
            Info(const std::pair<const TagTypeInfo, TagValue> &data);

            QString name() const;
            QString displayName() const;

            const TagTypeInfo& getTypeInfo() const;
            const TagValue& value() const;

            void setValue(const TagValue &);

        private:
            TagTypeInfo m_name;
            TagValue m_value;
    };
}

Q_DECLARE_METATYPE(TagTypeInfo)

#endif
