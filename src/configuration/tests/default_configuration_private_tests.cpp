
#include <gtest/gtest.h>

#include <QString>

#include "private/default_configuration_private.hpp"



//access to private parts of DefaultConfigurationPrivate
struct PublicWrapper: public DefaultConfigurationPrivate
{
    PublicWrapper(): DefaultConfigurationPrivate() {}

    bool useXml(const QString& str)
    {

    }
};


TEST(DefaultConfigurationPrivateShould, NotThrowAtConstruction)
{
    EXPECT_NO_THROW({
        DefaultConfigurationPrivate prv;
    });
}


TEST(DefaultConfigurationPrivateShould, ReturnNonEmptyConfigPath)
{
    DefaultConfigurationPrivate prv;

    const std::string path = prv.getConfigDir();

    ASSERT_EQ(false, path.empty());
}


TEST(DefaultConfigurationPrivateShould, ParseEmptyXmlWithoutError)
{
    DefaultConfigurationPrivate prv;

    const bool status = prv.useXml("");

    ASSERT_EQ(true, status);
}
