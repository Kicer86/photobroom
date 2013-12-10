
#include <gtest/gtest.h>

#include "private/default_configuration.hpp"
#include "entrydata.hpp"

TEST(DefaultConfigurationShould, beFilledWithKnownSetOfEntries)
{
    DefaultConfiguration configuration;
    
    const std::vector<Configuration::EntryData>& entries = configuration.getEntries();
    
    ASSERT_EQ(1, entries.size());
}


TEST(DefaultConfigurationShould, acceptEntries)
{
    DefaultConfiguration configuration;
    const int s = configuration.getEntries().size();
    
    configuration.addEntry(Configuration::EntryData("Test", "Entry"));
    
    auto set = configuration.getEntries();
    
    ASSERT_EQ(s + 1, set.size());
}


TEST(DefaultConfigurationShould, returnSavedEntry)
{
    DefaultConfiguration configuration;
    const int s = configuration.getEntries().size();
    
    configuration.addEntry(Configuration::EntryData("Test", "Entry"));
    
    const auto entry = configuration.findEntry(Configuration::ConfigurationKey("Test"));
    
    ASSERT_EQ(true, entry.is_initialized());
    ASSERT_EQ("Entry", entry->value());
    ASSERT_EQ("Test",  entry->key().getKeyRaw());
}
