
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
    
    data.setTag( TagNameInfo("test1"), TagValueInfo("test2") );
    
    const ITagData::TagsList &dataSet = data.getTags();
    
    CHECK_EQUAL(1, static_cast<int>(dataSet.size()));
    
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).name() == "test1");
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).valuesString() == "test2");
}


TEST(TagDataShould, overwritePreviousTagsValues)
{
    TagData data;
    
    data.setTag( TagNameInfo("test1"), TagValueInfo("test2") );
    data.setTag( TagNameInfo("test1"), TagValueInfo("test3") );
    
    const ITagData::TagsList& dataSet = data.getTags();
    
    CHECK_EQUAL(1, static_cast<int>(dataSet.size()));
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).name() == "test1");
    CHECK_EQUAL(true, ITagData::TagInfo(dataSet.begin()).valuesString() == "test3");
}


TEST(TagDataShould, beClearable)
{
    TagData data;
    
    data.setTag( TagNameInfo("test1"), TagValueInfo("test2") );
    data.setTag( TagNameInfo("test2"), TagValueInfo("test3") );
    data.clear();
    
    const ITagData::TagsList& dataSet = data.getTags();
    
    CHECK_EQUAL(0, static_cast<int>(dataSet.size()));
}


/*************************************************************************/

TEST_GROUP(TagDataCompositeShould)
{
    
};


TEST(TagDataCompositeShould, fillItsMinions)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
                             minion2(std::make_shared<TagData>()),
                             minion3(std::make_shared<TagData>());
    
    data.setTagDatas( {minion1, minion2, minion3} );
    data.setTag( TagNameInfo("name"), TagValueInfo("value") );
    
    auto m1 = minion1->getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL(true, ITagData::TagInfo(m1.begin()).name() == "name");
    CHECK_EQUAL(true, ITagData::TagInfo(m1.begin()).valuesString() == "value");
    
    auto m2 = minion2->getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL("name", ITagData::TagInfo(m2.begin()).name().toStdString());
    CHECK_EQUAL("value", ITagData::TagInfo(m2.begin()).valuesString().toStdString());
    
    auto m3 = minion3->getTags();
    CHECK_EQUAL(1, m1.size());
    CHECK_EQUAL("name", ITagData::TagInfo(m3.begin()).name().toStdString());
    CHECK_EQUAL("value", ITagData::TagInfo(m3.begin()).valuesString().toStdString());
}


TEST(TagDataCompositeShould, getTagsFromMinionsWhenAllDataAreTheSame)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
                             minion2(std::make_shared<TagData>()),
                             minion3(std::make_shared<TagData>());
    
    data.setTagDatas( {minion1, minion2, minion3} );
    
    minion1->setTag( TagNameInfo("name"), TagValueInfo("value") );
    minion2->setTag( TagNameInfo("name"), TagValueInfo("value") );
    minion3->setTag( TagNameInfo("name"), TagValueInfo("value") );
    
    auto info = data.getTags();
    
    CHECK_EQUAL(1, info.size());
}


TEST(TagDataCompositeShould, getOnlyCommonPartOfMinions)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
                             minion2(std::make_shared<TagData>()),
                             minion3(std::make_shared<TagData>());
    
    data.setTagDatas( {minion1, minion2, minion3} );
    
    minion1->setTag( TagNameInfo("name"), TagValueInfo("value") );
    minion2->setTag( TagNameInfo("name"), TagValueInfo("value") );
    minion3->setTag( TagNameInfo("name"), TagValueInfo("value") );
    
    minion1->setTag( TagNameInfo("name2"), TagValueInfo("value2") );
    minion2->setTag( TagNameInfo("name2"), TagValueInfo("value2") );
    minion3->setTag( TagNameInfo("name2"), TagValueInfo("value2") );
    
    minion1->setTag( TagNameInfo("name3"), TagValueInfo("value3") );
    minion2->setTag( TagNameInfo("name4"), TagValueInfo("value4") );
    minion3->setTag( TagNameInfo("name4"), TagValueInfo("value4") );
    
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

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
                             minion2(std::make_shared<TagData>()),
                             minion3(std::make_shared<TagData>());
    
    data.setTagDatas( {minion1, minion2, minion3} );
    
    minion1->setTag( TagNameInfo("name"), TagValueInfo("value_1") );
    minion2->setTag( TagNameInfo("name"), TagValueInfo("value_2") );
    minion3->setTag( TagNameInfo("name"), TagValueInfo("value_3") );
    
    minion1->setTag( TagNameInfo("name2"), TagValueInfo("value_1") );
    minion2->setTag( TagNameInfo("name2"), TagValueInfo("value_2") );
    minion3->setTag( TagNameInfo("name2"), TagValueInfo("value_3") );
    
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


TEST(TagDataCompositeShould, beClearable)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
                             minion2(std::make_shared<TagData>()),
                             minion3(std::make_shared<TagData>());
    
    data.setTagDatas( {minion1, minion2, minion3} );
    
    minion1->setTag( TagNameInfo("name"), TagValueInfo("value_1") );
    minion2->setTag( TagNameInfo("name"), TagValueInfo("value_2") );
    minion3->setTag( TagNameInfo("name"), TagValueInfo("value_3") );
    
    minion1->setTag( TagNameInfo("name2"), TagValueInfo("value_1") );
    minion2->setTag( TagNameInfo("name2"), TagValueInfo("value_2") );
    minion3->setTag( TagNameInfo("name2"), TagValueInfo("value_3") );
    
    data.clear();
    
    auto list = data.getTags();
    
    CHECK_EQUAL(0, list.size());
    CHECK_EQUAL(0, minion1->getTags().size());
    CHECK_EQUAL(0, minion1->getTags().size());
    CHECK_EQUAL(0, minion1->getTags().size());
}
