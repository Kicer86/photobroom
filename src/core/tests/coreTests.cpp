
#define CPPUTEST_MEM_LEAK_DETECTION_DISABLED

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
    
    data.setTag( ITagData::NameType("test1"), ITagData::ValueType("test2") );
    
    const ITagData::TagsList &dataSet = data.getTags();
    
    CHECK_EQUAL(1, static_cast<int>(dataSet.size()));
    
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).name() == "test1");
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).valuesString() == "test2");
}


TEST(TagDataShould, overwritePreviousTagsValues)
{
    TagData data;
    
    data.setTag( ITagData::NameType("test1"), ITagData::ValueType("test2") );
    data.setTag( ITagData::NameType("test1"), ITagData::ValueType("test3") );
    
    const ITagData::TagsList& dataSet = data.getTags();
    
    CHECK_EQUAL(1, static_cast<int>(dataSet.size()));
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).name() == "test1");
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).valuesString() == "test3");
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
    data.setTag( ITagData::NameType("name"), ITagData::ValueType("value") );
    
    auto m1 = minion1.getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL(true, ITagData::TagInfo(m1.begin()).name() == "name");
    CHECK_EQUAL(true, ITagData::TagInfo(m1.begin()).valuesString() == "value");
    
    auto m2 = minion2.getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL("name", ITagData::TagInfo(m2.begin()).name().toStdString());
    CHECK_EQUAL("value", ITagData::TagInfo(m2.begin()).valuesString().toStdString());
    
    auto m3 = minion3.getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL("name", ITagData::TagInfo(m3.begin()).name().toStdString());
    CHECK_EQUAL("value", ITagData::TagInfo(m3.begin()).valuesString().toStdString());
}


TEST(TagDataCompositeShould, getTagsFromMinionsWhenAllDataAreTheSame)
{
    TagDataComposite data;

    TagData minion1, minion2, minion3;
    
    data.setTagDatas( {&minion1, &minion2, &minion3} );
    
    minion1.setTag( ITagData::NameType("name"), ITagData::ValueType("value") );
    minion2.setTag( ITagData::NameType("name"), ITagData::ValueType("value") );
    minion3.setTag( ITagData::NameType("name"), ITagData::ValueType("value") );
    
    auto info = data.getTags();
    
    CHECK_EQUAL(1, info.size());
}


TEST(TagDataCompositeShould, getOnlyCommonPartOfMinions)
{
    TagDataComposite data;

    TagData minion1, minion2, minion3;
    
    data.setTagDatas( {&minion1, &minion2, &minion3} );
    
    minion1.setTag( ITagData::NameType("name"), ITagData::ValueType("value") );
    minion2.setTag( ITagData::NameType("name"), ITagData::ValueType("value") );
    minion3.setTag( ITagData::NameType("name"), ITagData::ValueType("value") );
    
    minion1.setTag( ITagData::NameType("name2"), ITagData::ValueType("value2") );
    minion2.setTag( ITagData::NameType("name2"), ITagData::ValueType("value2") );
    minion3.setTag( ITagData::NameType("name2"), ITagData::ValueType("value2") );
    
    minion1.setTag( ITagData::NameType("name3"), ITagData::ValueType("value3") );
    minion2.setTag( ITagData::NameType("name4"), ITagData::ValueType("value4") );
    minion3.setTag( ITagData::NameType("name4"), ITagData::ValueType("value4") );
    
    auto list = data.getTags();
    
    CHECK_EQUAL(2, list.size());
    
    auto it = list.begin();
    ITagData::TagInfo info(*it);
    CHECK_EQUAL("name", info.name().toStdString());
    CHECK_EQUAL("value", info.valuesString().toStdString());
    
    info = *(++it);
    CHECK_EQUAL("name2", info.name().toStdString());
    CHECK_EQUAL("value2", info.valuesString().toStdString());
}



TEST(TagDataCompositeShould, ReturnMergedValuesForCommonTagsWithDifferentValues)
{
    TagDataComposite data;

    TagData minion1, minion2, minion3;
    
    data.setTagDatas( {&minion1, &minion2, &minion3} );
    
    minion1.setTag( ITagData::NameType("name"), ITagData::ValueType("value_1") );
    minion2.setTag( ITagData::NameType("name"), ITagData::ValueType("value_2") );
    minion3.setTag( ITagData::NameType("name"), ITagData::ValueType("value_3") );
    
    minion1.setTag( ITagData::NameType("name2"), ITagData::ValueType("value_1") );
    minion2.setTag( ITagData::NameType("name2"), ITagData::ValueType("value_2") );
    minion3.setTag( ITagData::NameType("name2"), ITagData::ValueType("value_3") );
    
    auto list = data.getTags();
    
    CHECK_EQUAL(2, list.size());
    
    auto it = list.begin();
    ITagData::TagInfo info(*it);
    CHECK_EQUAL("name", info.name().toStdString());
    
    auto valIt = info.values().begin();
    CHECK_EQUAL("value_1", valIt->value().toStdString());
    ++valIt;
    CHECK_EQUAL("value_2", valIt->value().toStdString());
    ++valIt;
    CHECK_EQUAL("value_3", valIt->value().toStdString());
    
    info = *(++it);
    CHECK_EQUAL("name2", info.name().toStdString());
        
    valIt = info.values().begin();
    CHECK_EQUAL("value_1", valIt->value().toStdString());
    ++valIt;
    CHECK_EQUAL("value_2", valIt->value().toStdString());
    ++valIt;
    CHECK_EQUAL("value_3", valIt->value().toStdString());
}


