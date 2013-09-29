
#define CPPUTEST_MEM_LEAK_DETECTION_DISABLED

#include <CppUTest/TestHarness.h>

#include "../algo.hpp"

TEST_GROUP(MapIntersectionShould)
{
	
};
 


TEST(MapIntersectionShould, returnNullWhenOneMapIsNull)
{
    std::map<int, int> m1, m2;

    m1[0] = 1;
    m1[1] = 2;
    m1[2] = 3;
    
    auto result = Algo::map_intersection(m1, m2);
    
    CHECK_EQUAL(0, result.size());
}


TEST(MapIntersectionShould, returnOnlyCommonPartOfTwoMaps)
{
    std::map<int, int> m1, m2;

    m1[0] = 1;
    m1[1] = 2;
    m1[2] = 3;

    m2[0] = 1;
    m2[1] = 2;
    m2[3] = 4;

    auto result = Algo::map_intersection(m1, m2);

    CHECK_EQUAL(2, result.size());
}


TEST(MapIntersectionShould, returnOnlyCommonPartOfThreeMaps)
{
    std::map<int, int> m1, m2, m3;

    m1[0] = 1;
    m1[1] = 2;
    m1[2] = 3;

    m2[0] = 1;
    m2[1] = 2;
    m2[3] = 4;

    m3[0] = 1;
    m3[3] = 4;
    m3[4] = 5;

    auto result = Algo::map_intersection(m1, m2);
    result = Algo::map_intersection(result, m3);

    CHECK_EQUAL(1, result.size());
}


TEST(MapIntersectionShould, compareKeyOnly)
{
    std::map<int, int> m1, m2, m3;

    m1[0] = 101;
    m1[1] = 102;
    m1[2] = 103;

    m2[0] = 201;
    m2[1] = 202;
    m2[3] = 204;

    m3[0] = 301;
    m3[3] = 304;
    m3[4] = 305;

    auto result = Algo::map_intersection(m1, m2);
    result = Algo::map_intersection(result, m3);

    CHECK_EQUAL(1, result.size());
}

