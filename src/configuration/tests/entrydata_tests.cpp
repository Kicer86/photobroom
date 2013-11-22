
#define CPPUTEST_MEM_LEAK_DETECTION_DISABLED

#include <CppUTest/TestHarness.h>

#include <entrydata.hpp>


TEST_GROUP(EntryDataTests)
{
};

TEST(EntryDataTests, Should)
{

}


TEST_GROUP(ConfigurationKeyShould)
{
};


TEST(ConfigurationKeyShould, successfulyParseOneLevelKey)
{
    ConfigurationKey key;
    
    key.setKey("One");
    
    std::vector<std::string> result = key.getKey();
    
    CHECK_EQUAL(result.size(), 1);
    CHECK_EQUAL(result[0], "One");
}


TEST(ConfigurationKeyShould, successfulyParseTwoLevelKey)
{
    ConfigurationKey key;
    
    key.setKey("One::Two");
    
    std::vector<std::string> result = key.getKey();
    
    CHECK_EQUAL(result.size(), 2);
    CHECK_EQUAL(result[0], "One");
    CHECK_EQUAL(result[1], "Two");
}


TEST(ConfigurationKeyShould, successfulyParseThreeLevelKey)
{
    ConfigurationKey key;
    
    key.setKey("One::Two::Three");
    
    std::vector<std::string> result = key.getKey();
    
    CHECK_EQUAL(result.size(), 3);
    CHECK_EQUAL(result[0], "One");
    CHECK_EQUAL(result[1], "Two");
    CHECK_EQUAL(result[2], "Three");
}

