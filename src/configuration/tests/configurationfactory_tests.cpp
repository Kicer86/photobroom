
#include <gtest/gtest.h>

#include "configurationfactory.hpp"

TEST(ConfigurationFactoryShould, returnConfiguration)
{
    std::shared_ptr<IConfiguration> config = ConfigurationFactory::get();
    
    ASSERT_NE(nullptr, config.get());
}
