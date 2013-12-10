
#include <gtest/gtest.h>

#include "private/default_configuration.hpp"
#include "entrydata.hpp"

TEST(DefaultConfigurationShould, returnDefaultEntries)
{
    DefaultConfiguration configuration;
    
    const std::vector<Configuration::EntryData>& entries = configuration.getEntries();
    
    ASSERT_EQ(false, entries.empty());
}


TEST(DefaultConfigurationShould, acceptEntries)
{
    DefaultConfiguration configuration;

    configuration.addEntry(Configuration::EntryData("Test", "Entry"));
}
