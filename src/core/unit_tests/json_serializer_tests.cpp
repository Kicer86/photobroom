
#include <gtest/gtest.h>

#include "json_serializer_tests_r++.hpp"
#include "json_serializer.hpp"


namespace
{
    struct XYZ
    {
        int a = 7;
        int b = 8;

        auto operator<=>(const XYZ &) const = default;
    };
}

namespace JSon
{
    template<>
    struct CustomType<XYZ>
    {
        static QJsonObject serialize(const XYZ& xyz)
        {
            QJsonObject json;
            json["a"] = xyz.a;
            json["b"] = xyz.b;

            return json;
        }

        static XYZ deserialize(const QJsonObject& json)
        {
            XYZ xyz;

            xyz.a = json["a"].toInt();
            xyz.b = json["b"].toInt();

            return xyz;
        }
    };
}


TEST(JsonSerializerTest, SerializationDeserialization)
{
    DEF def;
    def.abc_vec.emplace_back(11, 15.f, -8., "World", QRect(3, 6, 9, 11));
    def.abc_list.emplace_back(-6, -765.f, 89., "World", QRect(-1, -50, 9, 11));

    const QJsonDocument json = JSon::serialize(def);
    const DEF def2 = JSon::deserialize<DEF>(json);

    EXPECT_EQ(def, def2);
}


TEST(JsonSerializerTest, CustomSerializationDeserialization)
{
    static_assert(JSon::impl::hasCustomSerialization<XYZ>);

    XYZ xyz;
    xyz.a = 77;
    xyz.b = -156;

    const QJsonDocument json = JSon::serialize(xyz);
    const XYZ xyz2 = JSon::deserialize<XYZ>(json);

    EXPECT_EQ(xyz, xyz2);
}


TEST(JsonSerializerTest, CustomSerializationDeserializationToArray)
{
    static_assert(JSon::impl::hasCustomSerialization<XYZ>);

    std::vector<XYZ> xyz;
    for(int i = 0; i < 5; i++)
        xyz.emplace_back(rand(), rand());

    const QJsonDocument json = JSon::serialize(xyz);
    const auto xyz2 = JSon::deserialize<std::vector<XYZ>>(json);

    EXPECT_EQ(xyz, xyz2);
}
