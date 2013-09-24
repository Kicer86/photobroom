
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
    
    const ITagData::TagsList &dataSet = data.getTags();
    
    CHECK_EQUAL(0, static_cast<int>(dataSet.size()));
}


TEST(TagDataShould, returnWhatWasInserted)
{
    TagData data;
    
    data.setTag("test1", "test2");
    
    const ITagData::TagsList &dataSet = data.getTags();
    
    CHECK_EQUAL(1, static_cast<int>(dataSet.size()));
    
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).name() == "test1");
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).values() == "test2");
}


TEST(TagDataShould, overwritePreviousTagsValues)
{
    TagData data;
    
    data.setTag("test1", "test2");
    data.setTag("test1", "test3");
    
    const ITagData::TagsList& dataSet = data.getTags();
    
    CHECK_EQUAL(1, static_cast<int>(dataSet.size()));
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).name() == "test1");
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).values() == "test3");
}



/*************************************************************************/

TEST_GROUP(TagDataCompositeShould)
{
    
};


TEST(TagDataCompositeShould, fillItsMinions)
{
    TagDataComposite data;

    TagData minion1, minion2, minion3;
    
    data.setTagDatas( {&minion1, &minion2, &minion3} );    
    data.setTag("name", "value");
    
    auto m1 = minion1.getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL(true, ITagData::TagInfo(m1.begin()).name() == "name");
    CHECK_EQUAL(true, ITagData::TagInfo(m1.begin()).values() == "value");
    
    auto m2 = minion2.getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL(true, ITagData::TagInfo(m2.begin()).name() == "name");
    CHECK_EQUAL(true, ITagData::TagInfo(m2.begin()).values() == "value");
    
    auto m3 = minion3.getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL(true, ITagData::TagInfo(m3.begin()).name() == "name");
    CHECK_EQUAL(true, ITagData::TagInfo(m3.begin()).values() == "value");
}


TEST(TagDataCompositeShould, getTagsFromMinionsWhenAllDataAreTheSame)
{
    TagDataComposite data;

    TagData minion1, minion2, minion3;
    
    data.setTagDatas( {&minion1, &minion2, &minion3} );
    
    minion1.setTag("name", "value");
    minion2.setTag("name", "value");
    minion3.setTag("name", "value");
    
    auto info = data.getTags();
    
    CHECK_EQUAL(2, info.size());
}
