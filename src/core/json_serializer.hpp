
#ifndef JSON_SERIALIZER_HPP_INCLUDED
#define JSON_SERIALIZER_HPP_INCLUDED

#include <QDataStream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "generic_concepts.hpp"



namespace JSon
{
    template<typename T>
    struct CustomType;

    namespace impl
    {
        template<typename T, typename R>
        concept hasAssignmentOperator = requires(T a, const R b)
        {
            { a = b };
        };

        template<typename T>
        concept isQtStreamable = requires(QDataStream s, T a)
        {
            { s << a };
            { s >> a };
        };

        template<typename T>
        concept hasCustomSerialization = requires(T a)
        {
            { JSon::CustomType<T>::serialize(a) } -> std::same_as<typename JSon::CustomType<T>::type>;
            { JSon::CustomType<T>::deserialize({}) } -> std::same_as<T>;
        };

        template<typename JT, typename T>
        JT convertTo(const T& valueRef)
        {
            if constexpr (std::is_same_v<JT, QJsonArray>)
            {
                assert(valueRef.isArray());

                QJsonArray array;
                if constexpr (std::is_same_v<T, QJsonDocument>)
                    array = valueRef.array();
                else
                    array = valueRef.toArray();

                return array;
            }
            else if constexpr (std::is_same_v<JT, QJsonObject>)
            {
                assert(valueRef.isObject());

                QJsonObject object;
                if constexpr (std::is_same_v<T, QJsonDocument>)
                    object = valueRef.object();
                else
                    object = valueRef.toObject();

                return object;
            }
            else
                static_assert(false, "Unexpected type");
        }


        template<typename T>
        auto getSerialized(const T &);

        template<typename T>
        auto serialize(const T& obj)
        {
            if constexpr (hasCustomSerialization<T>)
                return JSon::CustomType<T>::serialize(obj);
            else if constexpr (Container<T>)
            {
                QJsonArray jsonArr;

                for(const auto& i: obj)
                    jsonArr.append(getSerialized(i));

                return jsonArr;
            }
            else
            {
                QJsonObject jsonObj;

                reflectpp::get_object_members(obj, [&jsonObj](auto member_info, const auto& member)
                {
                    const QString name = QString::fromStdString(std::string(member_info.name));
                    jsonObj[name] = getSerialized(member);
                });

                return jsonObj;
            }
        }

        template<typename T>
        auto getSerialized(const T& obj)
        {
            if constexpr (hasAssignmentOperator<QJsonValueRef, T>)
                return obj;
            else if constexpr (isQtStreamable<T>)
            {
                QByteArray d;
                QDataStream s(&d, QIODeviceBase::WriteOnly);
                s << obj;
                return d.toBase64().data();
            }
            else
                return serialize(obj);
        }

        template<typename T, typename JT>
        T getDeserialized(const JT &);

        template<typename T, typename JT>
        T deserialize(const JT& json)
        {
            T r;

            if constexpr (hasCustomSerialization<T>)
                return JSon::CustomType<T>::deserialize(convertTo<typename JSon::CustomType<T>::type>(json));
            else if constexpr (Container<T>)
            {
                const QJsonArray array = convertTo<QJsonArray>(json);

                using VT = typename T::value_type;

                for(const auto& e: array)
                    r.push_back(getDeserialized<VT>(e));
            }
            else
            {
                const QJsonObject object = convertTo<QJsonObject>(json);

                reflectpp::set_object_members(r, [&object](const auto member_info)
                {
                    using VT = decltype(member_info)::type;
                    const QString name = QString::fromStdString(std::string(member_info.name));
                    const auto value = object[name];

                    return getDeserialized<VT>(value);
                });
            }

            return r;
        }

        template<typename T, typename JT>
        T getDeserialized(const JT& value)
        {
            if constexpr (hasAssignmentOperator<QJsonValueRef, T>)
                return value.toVariant().template value<T>();
            else if constexpr (isQtStreamable<T>)
            {
                QByteArray d = QByteArray::fromBase64(value.toString().toUtf8());
                QDataStream s(&d, QIODeviceBase::ReadOnly);
                T obj;
                s >> obj;
                return obj;
            }
            else
                return deserialize<T>(value);
        }
    }

    template<typename T>
    QJsonDocument serialize(const T& obj)
    {
        const auto jsonObj = impl::serialize(obj);
        const QJsonDocument doc(jsonObj);

        return doc;
    }

    template<typename T>
    T deserialize(const QJsonDocument& doc)
    {
        return impl::deserialize<T>(doc);
    }
}

#endif
