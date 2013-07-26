
#include <CppUTest/TestHarness.h>

#include "../types.hpp"

TEST_GROUP(TagDataShould)
{
	
};


TEST(TagDataShould, beConstructable)
{
	TagData data;
}


TEST(TagDataShould, returnEmptySetWhenConstructed)
{
    TagData data;
    
    std::vector<TagData::TagInfo> dataSet = data.getTags();
    
    CHECK_EQUAL(0, static_cast<int>(dataSet.size()));
}
