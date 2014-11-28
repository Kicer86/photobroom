
#include <gtest/gtest.h>

#include "tag.hpp"

/*
TEST(TagDataShould, beConstructable)
{
    TagData data;
}


TEST(TagDataShould, returnEmptySetWhenConstructed)
{
    TagData data;

    const ITagData::TagsList &dataSet = data.getTags();

    ASSERT_EQ(0, static_cast<int>(dataSet.size()));
}


TEST(TagDataShould, returnWhatWasInserted)
{
    TagData data;

    data.setTag( TagNameInfo("test1", TagNameInfo::Text), TagValueInfo("test2") );

    const ITagData::TagsList &dataSet = data.getTags();

    ASSERT_EQ(1, static_cast<int>(dataSet.size()));

    ASSERT_EQ(true, ITagData::TagInfo(dataSet.begin()).name() == "test1");
    ASSERT_EQ(true, ITagData::TagInfo(dataSet.begin()).valuesString() == "test2");
}


TEST(TagDataShould, overwritePreviousTagsValues)
{
    TagData data;

    data.setTag( TagNameInfo("test1", TagNameInfo::Text), TagValueInfo("test2") );
    data.setTag( TagNameInfo("test1", TagNameInfo::Text), TagValueInfo("test3") );

    const ITagData::TagsList& dataSet = data.getTags();

    ASSERT_EQ(1, static_cast<int>(dataSet.size()));
    ASSERT_EQ(true, ITagData::TagInfo(dataSet.begin()).name() == "test1");
    ASSERT_EQ(true, ITagData::TagInfo(dataSet.begin()).valuesString() == "test3");
}


TEST(TagDataShould, beClearable)
{
    TagData data;

    data.setTag( TagNameInfo("test1", TagNameInfo::Text), TagValueInfo("test2") );
    data.setTag( TagNameInfo("test2", TagNameInfo::Text), TagValueInfo("test3") );
    data.clear();

    const ITagData::TagsList& dataSet = data.getTags();

    ASSERT_EQ(0, static_cast<int>(dataSet.size()));
}
*/

/*************************************************************************/

/*
TEST(TagDataCompositeShould, fillItsMinions)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
        minion2(std::make_shared<TagData>()),
        minion3(std::make_shared<TagData>());

    data.setTagDatas( {minion1, minion2, minion3} );
    data.setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value") );

    auto m1 = minion1->getTags();
    ASSERT_EQ(1, m1.size());
    ASSERT_EQ(true, ITagData::TagInfo(m1.begin()).name() == "name");
    ASSERT_EQ(true, ITagData::TagInfo(m1.begin()).valuesString() == "value");

    auto m2 = minion2->getTags();
    ASSERT_EQ(1, m1.size());
    ASSERT_EQ("name", ITagData::TagInfo(m2.begin()).name().toStdString());
    ASSERT_EQ("value", ITagData::TagInfo(m2.begin()).valuesString().toStdString());

    auto m3 = minion3->getTags();
    ASSERT_EQ(1, m1.size());
    ASSERT_EQ("name", ITagData::TagInfo(m3.begin()).name().toStdString());
    ASSERT_EQ("value", ITagData::TagInfo(m3.begin()).valuesString().toStdString());
}


TEST(TagDataCompositeShould, getTagsFromMinionsWhenAllDataAreTheSame)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
        minion2(std::make_shared<TagData>()),
        minion3(std::make_shared<TagData>());

    data.setTagDatas( {minion1, minion2, minion3} );

    minion1->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value") );
    minion2->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value") );
    minion3->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value") );

    auto info = data.getTags();

    ASSERT_EQ(1, info.size());
}


TEST(TagDataCompositeShould, getOnlyCommonPartOfMinions)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
        minion2(std::make_shared<TagData>()),
        minion3(std::make_shared<TagData>());

    data.setTagDatas( {minion1, minion2, minion3} );

    minion1->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value") );
    minion2->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value") );
    minion3->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value") );

    minion1->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value2") );
    minion2->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value2") );
    minion3->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value2") );

    minion1->setTag( TagNameInfo("name3", TagNameInfo::Text), TagValueInfo("value3") );
    minion2->setTag( TagNameInfo("name4", TagNameInfo::Text), TagValueInfo("value4") );
    minion3->setTag( TagNameInfo("name4", TagNameInfo::Text), TagValueInfo("value4") );

    auto list = data.getTags();

    ASSERT_EQ(2, list.size());

    auto it = list.begin();
    ITagData::TagInfo info(*it);
    ASSERT_EQ("name", info.name().toStdString());
    ASSERT_EQ("value", info.valuesString().toStdString());

    info = *(++it);
    ASSERT_EQ("name2", info.name().toStdString());
    ASSERT_EQ("value2", info.valuesString().toStdString());
}


TEST(TagDataCompositeShould, ReturnMergedValuesForCommonTagsWithDifferentValues)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
        minion2(std::make_shared<TagData>()),
        minion3(std::make_shared<TagData>());

    data.setTagDatas( {minion1, minion2, minion3} );

    minion1->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value_1") );
    minion2->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value_2") );
    minion3->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value_3") );

    minion1->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value_1") );
    minion2->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value_2") );
    minion3->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value_3") );

    auto list = data.getTags();

    ASSERT_EQ(2, list.size());

    auto it = list.begin();
    ITagData::TagInfo info(*it);
    ASSERT_EQ("name", info.name().toStdString());

    auto valIt = info.values().begin();
    ASSERT_EQ("value_1", valIt->value().toStdString());
    ++valIt;
    ASSERT_EQ("value_2", valIt->value().toStdString());
    ++valIt;
    ASSERT_EQ("value_3", valIt->value().toStdString());

    info = *(++it);
    ASSERT_EQ("name2", info.name().toStdString());

    valIt = info.values().begin();
    ASSERT_EQ("value_1", valIt->value().toStdString());
    ++valIt;
    ASSERT_EQ("value_2", valIt->value().toStdString());
    ++valIt;
    ASSERT_EQ("value_3", valIt->value().toStdString());
}


TEST(TagDataCompositeShould, beClearable)
{
    TagDataComposite data;

    std::shared_ptr<TagData> minion1(std::make_shared<TagData>()),
        minion2(std::make_shared<TagData>()),
        minion3(std::make_shared<TagData>());

    data.setTagDatas( {minion1, minion2, minion3} );

    minion1->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value_1") );
    minion2->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value_2") );
    minion3->setTag( TagNameInfo("name", TagNameInfo::Text), TagValueInfo("value_3") );

    minion1->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value_1") );
    minion2->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value_2") );
    minion3->setTag( TagNameInfo("name2", TagNameInfo::Text), TagValueInfo("value_3") );

    data.clear();

    auto list = data.getTags();

    ASSERT_EQ(0, list.size());
    ASSERT_EQ(0, minion1->getTags().size());
    ASSERT_EQ(0, minion1->getTags().size());
    ASSERT_EQ(0, minion1->getTags().size());
}
*/
