
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
    CHECK_EQUAL("One", result[0]);
}


TEST(ConfigurationKeyShould, successfulyParseTwoLevelKey)
{
    ConfigurationKey key;
    
    key.setKey("One::Two");
    
    std::vector<std::string> result = key.getKey();
    
    CHECK_EQUAL(result.size(), 2);
    CHECK_EQUAL("One", result[0]);
    CHECK_EQUAL("Two", result[1]);
}


TEST(ConfigurationKeyShould, successfulyParseThreeLevelKey)
{
    ConfigurationKey key;
    
    key.setKey("One::Two::Three");
    
    std::vector<std::string> result = key.getKey();
    
    CHECK_EQUAL(result.size(), 3);
    CHECK_EQUAL("One", result[0]);
    CHECK_EQUAL("Two", result[1]);
    CHECK_EQUAL("Three", result[2]);
}

TEST(ConfigurationKeyShould, returnResultInRawFormat)
{
    ConfigurationKey key;
    
    key.setKey("One::Two::Three");
    
    std::string result = key.getKeyRaw();
    
    CHECK_EQUAL("One::Two::Three", result);
}

TEST(ConfigurationKeyShould, acceptFormattedInput)
{
    ConfigurationKey key;
    
    key.setKey({"One", "Two", "Three"});
    
    std::string result = key.getKeyRaw();
    
    CHECK_EQUAL("One::Two::Three", result);
}
