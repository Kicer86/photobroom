
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

    struct IJK
    {
        std::vector<int> a;

        auto operator<=>(const IJK &) const = default;
    };
}

namespace JSon
{
    template<>
    struct CustomType<XYZ>
    {
        using type = QJsonObject;

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

    template<>
    struct CustomType<IJK>
    {
        using type = QJsonArray;

        static QJsonArray serialize(const IJK& ijk)
        {
            QJsonArray array;
            std::ranges::copy(ijk.a, std::back_inserter(array));

            return array;
        }

        static IJK deserialize(const QJsonArray& json)
        {
            IJK ijk;

            std::transform(json.begin(), json.end(), std::back_inserter(ijk.a), [](const auto& value) {return value.toInt();});

            return ijk;
        }
    };
}


static_assert(JSon::impl::hasCustomSerialization<XYZ>);


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

    XYZ xyz;
    xyz.a = 77;
    xyz.b = -156;

    const QJsonDocument json = JSon::serialize(xyz);
    const XYZ xyz2 = JSon::deserialize<XYZ>(json);

    EXPECT_EQ(xyz, xyz2);
}


TEST(JsonSerializerTest, CustomSerializationDeserializationOfArray)
{
    std::vector<XYZ> xyz;
    for(int i = 0; i < 5; i++)
        xyz.emplace_back(rand(), rand());

    const QJsonDocument json = JSon::serialize(xyz);
    const auto xyz2 = JSon::deserialize<std::vector<XYZ>>(json);

    EXPECT_EQ(xyz, xyz2);
}


TEST(JsonSerializerTest, CustomArraySerializationDeserialization)
{
    IJK ijk;
    ijk.a.push_back(1);
    ijk.a.push_back(2);
    ijk.a.push_back(555);

    const QJsonDocument json = JSon::serialize(ijk);
    const auto ijk2 = JSon::deserialize<IJK>(json);

    EXPECT_EQ(ijk, ijk2);
}


TEST(JsonSerializerTest, CustomTypeSerialization)
{
    const XYZ xyz;
    const auto json = JSon::serialize(xyz);

    const QJsonDocument expectedJson = QJsonDocument::fromJson(
        R"(
            {
                "a": 7,
                "b": 8
            }
        )"
    );

    EXPECT_EQ(json, expectedJson);
}


TEST(JsonSerializerTest, ArrayOfCustomTypeSerialization)
{
    const std::vector<XYZ> xyz({ {1, 2}, {-5, -9}, {-100, 800} });
    const auto json = JSon::serialize(xyz);

    const QJsonDocument expectedJson = QJsonDocument::fromJson(
        R"(
            [
                {
                    "a": 1,    "b": 2
                },
                {
                    "a": -5,   "b": -9
                },
                {
                    "a": -100, "b": 800
                }
            ]
        )"
    );

    EXPECT_EQ(json, expectedJson);
}


TEST(JsonSerializerTest, ArrayOfSimpleTypeSerialization)
{
    const std::vector<int> xyz({-10, -5, 1, 9, -500, 9});
    const auto json = JSon::serialize(xyz);

    const QJsonDocument expectedJson = QJsonDocument::fromJson(
        R"(
            [
                -10, -5, 1, 9, -500, 9
            ]
        )"
    );

    EXPECT_EQ(json, expectedJson);
}
