
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


TEST(TagDataShould, returnWhatWasInserted)
{
    TagData data;
    
    data.setTag("test1", "test2");
    
    std::vector<TagData::TagInfo> dataSet = data.getTags();
    
    CHECK_EQUAL(1, static_cast<int>(dataSet.size()));
    
    CHECK_EQUAL("test1", dataSet[0].name);
    CHECK_EQUAL("test2", dataSet[0].values);
}


TEST(TagDataShould, overwritePreviousTagsValues)
{
    TagData data;
    
    data.setTag("test1", "test2");
    data.setTag("test1", "test3");
    
    std::vector<TagData::TagInfo> dataSet = data.getTags();
    
    CHECK_EQUAL(1, static_cast<int>(dataSet.size()));
    CHECK_EQUAL("test1", dataSet[0].name);
    CHECK_EQUAL("test3", dataSet[0].values);
}
