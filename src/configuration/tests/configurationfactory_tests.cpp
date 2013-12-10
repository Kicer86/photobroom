
#include <gtest/gtest.h>

#include "configurationfactory.hpp"

TEST(ConfigurationFactoryShould, returnConfiguration)
{
    std::shared_ptr<IConfiguration> config = ConfigurationFactory::get();
    
    ASSERT_NE(nullptr, config.get());
}


TEST(ConfigurationFactoryShould, alwaysReturnTheSameConfiguration)
{
    std::shared_ptr<IConfiguration> config1 = ConfigurationFactory::get();
    std::shared_ptr<IConfiguration> config2 = ConfigurationFactory::get();
    
    ASSERT_EQ(config1.get(), config2.get());
}

