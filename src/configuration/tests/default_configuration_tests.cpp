
#include <gtest/gtest.h>

#include "private/default_configuration.hpp"

TEST(DefaultConfigurationShould, returnDefaultEntries)
{
    DefaultConfiguration configuration;
    
    const std::vector<const EntryData>& entries = configuration.getEntries();
    
    ASSERT_NE(0L, entries.size());
}
