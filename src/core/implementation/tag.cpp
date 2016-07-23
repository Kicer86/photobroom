
#include "tag.hpp"

#include <QDate>
#include <QString>
#include <QStringList>
#include <QTime>

#include "base_tags.hpp"
#include <variant_converter.hpp>


TagNameInfo::TagNameInfo(): name(), displayName(), type(Invalid)
{

}


TagNameInfo::TagNameInfo(const QString& n, const TagNameInfo::Type t, const QString& d): name(n), displayName(d), type(t)
{

}



TagNameInfo::TagNameInfo(const QString& n, const Type t): name(n), displayName(dn(n)), type(t)
{

}


TagNameInfo::TagNameInfo(const QString& n, int t): TagNameInfo(n, Type(t))
{

}


TagNameInfo::TagNameInfo(const TagNameInfo& other): name(other.name), displayName(other.displayName), type(other.type)
{

}


TagNameInfo::operator QString() const
{
    return name;
}


bool TagNameInfo::operator==(const TagNameInfo& other) const
{
    const bool result = name == other.name;

    return result;
}


bool TagNameInfo::operator<(const TagNameInfo& other) const
{
    const bool result = name < other.name;

    return result;
}


bool TagNameInfo::operator>(const TagNameInfo& other) const
{
    const bool result = name > other.name;

    return result;
}


TagNameInfo& TagNameInfo::operator=(const TagNameInfo& other)
{
    name = other.name;
    type = other.type;
    displayName = other.displayName;

    return *this;
}


const QString& TagNameInfo::getName() const
{
    return name;
}


const QString& TagNameInfo::getDisplayName() const
{
    return displayName;
}



TagNameInfo::Type TagNameInfo::getType() const
{
    return type;
}


QString TagNameInfo::dn(const QString& n) const
{
    // If name is base name, then we have translation.
    // Otherwise we are working with user defined tag

    const TagNameInfo* baseTag = nullptr;
    const auto& all = BaseTags::getAll();

    for(const auto& tag: all)
    {
        if (n == tag)
        {
            baseTag = &tag;
            break;
        }
    }

    //use translation for base. For user's tags use name directly
    const QString result = baseTag == nullptr? n: baseTag->displayName;

    return result;
}


//////////////////////////////////////////////////////////////


TagValue::TagValue(): m_type(Type::Empty), m_value(nullptr)
{

}


TagValue::TagValue(const TagValue& other): TagValue()
{
    copy(other);
}


TagValue::TagValue(const QVariant& value): TagValue()
{
    set(value);
}


TagValue::~TagValue()
{
    destroyValue();
}


TagValue& TagValue::operator=(const TagValue& other)
{
    copy(other);

    return *this;
}


void TagValue::set(const QVariant& v)
{
    const QVariant::Type type = v.type();

    switch(type)
    {
        default:
            assert(!"unknown type");

        case QVariant::String:
            set(v.toString());
            break;

        case QVariant::Date:
            set(v.toDate());
            break;

        case QVariant::Time:
            set(v.toTime());
            break;

        case QVariant::StringList:
        {
            const QStringList l = v.toStringList();

            std::deque<TagValue> values;

            for(const QString& str: l)
            {
                const TagValue tagValue(str);

                values.push_back(tagValue);
            }

            set(values);
            break;
        }
    }
}


void TagValue::set(const QDate& date)
{
    destroyValue();

    QDate* v = new QDate(date);
    m_value = v;
    m_type = Type::Date;
}


void TagValue::set(const QTime& time)
{
    destroyValue();

    QTime* v = new QTime(time);
    m_value = v;
    m_type = Type::Time;
}


void TagValue::set(const std::deque<TagValue>& values)
{
    destroyValue();

    std::deque<TagValue>* v = new std::deque<TagValue>(values);
    m_value = v;
    m_type = Type::List;
}


void TagValue::set(const QString& string)
{
    destroyValue();

    QString* v = new QString(string);
    m_value = v;
    m_type = Type::String;
}


QVariant TagValue::get() const
{
    QVariant result;

    switch (m_type)
    {
        case Type::Empty:
            break;

        case Type::Date:
            result = * get<TagValueTraits<Type::Date>::StorageType>();
            break;

        case Type::List:
        {
            QStringList localResult;
            std::deque<TagValue>* v = get<TagValueTraits<Type::List>::StorageType>();

            for(const TagValue& tagValue: *v)
                localResult.append(tagValue.string());

            result = localResult;
            break;
        }

        case Type::String:
            result = * get<TagValueTraits<Type::String>::StorageType>();
            break;

        case Type::Time:
            result = * get<TagValueTraits<Type::Time>::StorageType>();
            break;
    }

    return result;
}


QDate TagValue::getDate() const
{
    auto* v = get<TagValueTraits<Type::Date>::StorageType>();

    return *v;
}


std::deque<TagValue> TagValue::getList() const
{
    auto* v = get<TagValueTraits<Type::List>::StorageType>();

    return *v;
}


QString TagValue::getString() const
{
    auto* v = get<TagValueTraits<Type::String>::StorageType>();

    return *v;
}


QTime TagValue::getTime() const
{
    auto* v = get<TagValueTraits<Type::Time>::StorageType>();

    return *v;
}


TagValue::Type TagValue::type() const
{
    return m_type;
}


bool TagValue::operator==(const TagValue& other) const
{
    const QString thisString = string();
    const QString otherString = other.string();

    return thisString == otherString;
}


bool TagValue::operator!=(const TagValue& other) const
{
    const QString thisString = string();
    const QString otherString = other.string();

    return thisString != otherString;
}


bool TagValue::operator<(const TagValue& other) const
{
    const QString thisString = string();
    const QString otherString = other.string();

    return thisString < otherString;
}


void TagValue::destroyValue()
{
    switch (m_type)
    {
        case Type::Empty:
            break;

        case Type::Date:
            delete get<TagValueTraits<Type::Date>::StorageType>();
            break;

        case Type::List:
        {
            auto* v = get<TagValueTraits<Type::List>::StorageType>();

            for(TagValue& tagValue: *v)
                tagValue.destroyValue();

            delete v;
            break;
        }

        case Type::String:
            delete get<TagValueTraits<Type::String>::StorageType>();
            break;

        case Type::Time:
            delete get<TagValueTraits<Type::Time>::StorageType>();
            break;
    }

    m_type = Type::Empty;
    m_value = nullptr;
}


void TagValue::copy(const TagValue& other)
{
    switch (other.m_type)
    {
        case Type::Empty:
            break;

        case Type::Date:
            set(* other.get<TagValueTraits<Type::Date>::StorageType>() );
            break;

        case Type::List:
            set(* other.get<TagValueTraits<Type::List>::StorageType>() );
            break;

        case Type::String:
            set(* other.get<TagValueTraits<Type::String>::StorageType>() );
            break;

        case Type::Time:
            set(* other.get<TagValueTraits<Type::Time>::StorageType>() );
            break;
    }

    m_type = other.m_type;
}


template<>
bool TagValue::validate<QDate>() const
{
    return m_type == Type::Date && m_value != nullptr;
}


template<>
bool TagValue::validate<QTime>() const
{
    return m_type == Type::Time && m_value != nullptr;
}


template<>
bool TagValue::validate<QString>() const
{
    return m_type == Type::String && m_value != nullptr;
}


template<>
bool TagValue::validate<std::deque<TagValue>>() const
{
    return m_type == Type::List && m_value != nullptr;
}


QString TagValue::string() const
{
    QString result;

    switch(m_type)
    {
        case Type::Empty:
            break;

        case Type::Date:
        {
            QDate* v = get<TagValueTraits<Type::Date>::StorageType>();
            result = v->toString("yyyy.MM.dd");;
            break;
        }

        case Type::List:
        {
            QString localResult;
            auto* v = get<TagValueTraits<Type::List>::StorageType>();

            for(const TagValue& tagValue: *v)
                localResult += tagValue.string() + '\0';

            result = localResult;
            break;
        }

        case Type::String:
        {
            QString* v = get<TagValueTraits<Type::String>::StorageType>();
            result = *v;
            break;
        }

        case Type::Time:
        {
            QTime* v = get<TagValueTraits<Type::Time>::StorageType>();
            result = v->toString("HH:mm:ss");
            break;
        }
    }

    return result;
}


//////////////////////////////////////////////////////////////


namespace Tag
{

    Info::Info(const Tag::TagsList::const_iterator &it): m_name(it->first), m_value(it->second)
    {

    }

    Info::Info(const std::pair<const TagNameInfo, TagValue> &data): m_name(data.first), m_value(data.second)
    {

    }

    Info::Info(const TagNameInfo& n, const QVariant& v): m_name(n), m_value()
    {
        setValue(v);
    }

    Info& Info::operator=(const std::pair<TagNameInfo, TagValue> &data)
    {
        m_name = data.first;
        m_value = data.second;

        return *this;
    }

    const QString& Info::name() const
    {
        return m_name.getName();
    }

    const QString& Info::displayName() const
    {
        return m_name.getDisplayName();
    }

    const TagNameInfo& Info::getTypeInfo() const
    {
        return m_name;
    }

    const TagValue& Info::value() const
    {
        return m_value;
    }

    void Info::setValue(const QVariant& v)
    {
        m_value.set(v);
    }

}
