
#include <gtest/gtest.h>

#include "entrydata.hpp"


TEST(ConfigurationKeyShould, successfulyParseOneLevelKey)
{
    ConfigurationKey key;
    
    key.setKey("One");
    
    std::vector<std::string> result = key.getKey();
    
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ("One", result[0]);
}


TEST(ConfigurationKeyShould, takeKeyValueWhileBeingConstructed)
{
    ConfigurationKey key("One");
        
    std::vector<std::string> result = key.getKey();
    
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ("One", result[0]);
}


TEST(ConfigurationKeyShould, takeStringWithKeyValueWhileBeingConstructed)
{
    const std::string keyValue("One");
    ConfigurationKey key(keyValue);
        
    std::vector<std::string> result = key.getKey();
    
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ("One", result[0]);
}


TEST(ConfigurationKeyShould, successfulyParseTwoLevelKey)
{
    ConfigurationKey key;
    
    key.setKey("One::Two");
    
    std::vector<std::string> result = key.getKey();
    
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ("One", result[0]);
    ASSERT_EQ("Two", result[1]);
}


TEST(ConfigurationKeyShould, successfulyParseThreeLevelKey)
{
    ConfigurationKey key;
    
    key.setKey("One::Two::Three");
    
    std::vector<std::string> result = key.getKey();
    
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ("One", result[0]);
    ASSERT_EQ("Two", result[1]);
    ASSERT_EQ("Three", result[2]);
}

TEST(ConfigurationKeyShould, returnResultInRawFormat)
{
    ConfigurationKey key;
    
    key.setKey("One::Two::Three");
    
    std::string result = key.getKeyRaw();
    
    ASSERT_EQ("One::Two::Three", result);
}

TEST(ConfigurationKeyShould, acceptFormattedInput)
{
    ConfigurationKey key;
    
    key.setKey({"One", "Two", "Three"});
    
    std::string result = key.getKeyRaw();
    
    ASSERT_EQ("One::Two::Three", result);
}

///////////////////////////////////////////////////////

TEST(EntryDataShould, beConstructable)
{
    EntryData entry;
}


TEST(EntryDataShould, returnTrueForComparisonOfTwoEmptyEntries)
{
    EntryData entry1, entry2;
    
    ASSERT_EQ(entry1, entry2);
}


TEST(EntryDataShould, acceptKeyAndValueWileBeingConstructed)
{
    EntryData entry("a::b::c", "value");
}
