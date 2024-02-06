
#ifndef JSON_SERIALIZER_HPP_INCLUDED
#define JSON_SERIALIZER_HPP_INCLUDED

#include <QDataStream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "rfl.hpp"
#include "rfl/to_view.hpp"
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
                assert(!"Unexpected type");
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

                const auto objView = rfl::to_view(obj);

                const auto process = [&jsonObj]<class... F>(F... _field) {
                    (( jsonObj[QString::fromStdString(typename F::Name().str())] = getSerialized(*_field.value()) ), ...);
                };

                std::apply(process, objView.fields());

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
                return QString(d.toBase64());
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
                const QJsonObject jsonObj = convertTo<QJsonObject>(json);
                auto objView = rfl::to_view(r);

                const auto process = [&jsonObj]<class... F>(F... _field) {
                    (( *_field.value() = getDeserialized<std::remove_pointer_t<typename F::Type>>(jsonObj[QString::fromStdString(typename F::Name().str())]) ), ...);
                };

                std::apply(process, objView.fields());
            }

            return r;
        }

        template<typename T, typename JT>
        T getDeserialized(const JT& value)
        {
            if (value.isUndefined())
                return {};
            else if constexpr (hasAssignmentOperator<QJsonValueRef, T>)
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

    template<typename T>
    T deserialize(const QJsonValue& value)
    {
        return impl::deserialize<T>(value);
    }
}

#endif
