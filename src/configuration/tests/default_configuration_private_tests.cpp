
#include <gtest/gtest.h>

#include <QString>
#include <QXmlStreamReader>

#include "private/default_configuration_private.hpp"
#include "entrydata.hpp"


//access to private parts of DefaultConfigurationPrivate
struct PublicWrapper: public DefaultConfigurationPrivate
{
    PublicWrapper(): DefaultConfigurationPrivate() {}

    bool parseXml_Keys(QXmlStreamReader* reader)
    {
        return DefaultConfigurationPrivate::parseXml_Keys(reader);
    }

    bool parseXml_DefaultKeys(QXmlStreamReader* reader)
    {
        return DefaultConfigurationPrivate::parseXml_DefaultKeys(reader);
    }

    const std::unordered_set<Configuration2::ConfigurationKey, hash>& getKnownKeys() const
    {
        return m_known_keys;
    }

    const std::unordered_map<Configuration2::ConfigurationKey, Configuration2::EntryData, hash>& getData() const
    {
        return m_data;
    }

};


TEST(DefaultConfigurationPrivateShould, NotThrowAtConstruction)
{
    EXPECT_NO_THROW(
    {
        DefaultConfigurationPrivate prv;
    });
}


TEST(DefaultConfigurationPrivateShould, ReturnNonEmptyConfigPath)
{
    DefaultConfigurationPrivate prv;

    const QString path = prv.getConfigDir();

    ASSERT_EQ(false, path.isEmpty());
}


TEST(DefaultConfigurationPrivateShould, ParseEmptyXmlWithoutError)
{
    DefaultConfigurationPrivate prv;

    const bool status = prv.useXml("");

    ASSERT_EQ(true, status);
}



TEST(DefaultConfigurationPrivateShould, ParseSimplestXmlWithoutError)
{
    DefaultConfigurationPrivate prv;

    const bool status = prv.useXml("<configuration />");
    ASSERT_EQ(true, status);

    const bool status2 = prv.useXml("<configuration></configuration>");
    ASSERT_EQ(true, status2);
}



TEST(DefaultConfigurationPrivateShould, LoadProvidedDefaultsAndKeys)
{
    const std::string xml =
        "<configuration>"
        "    <keys>"
        "        <key name='4' />"
        "    </keys>"
        "    <defaults>"
        "        <key name='1' value='2'/>"
        "        <key name='2' value='4'/>"
        "        <key name='3' value='6'/>"
        "    </defaults>"
        "</configuration>";

    PublicWrapper prv;
    const bool status = prv.useXml(xml.c_str());
    ASSERT_EQ(true, status);

    ASSERT_EQ(4, prv.getKnownKeys().size());      //four introduced keys
    ASSERT_EQ(3, prv.getData().size());           //3 default entries

    auto data = prv.getData();
    ASSERT_EQ(Configuration2::EntryData("1", "2"), data["1"]);
    ASSERT_EQ(Configuration2::EntryData("2", "4"), data["2"]);
    ASSERT_EQ(Configuration2::EntryData("3", "6"), data["3"]);
}

/********************************************************************************/

struct DefaultConfigurationPrivateFunctionsShould: public ::testing::Test
{
    DefaultConfigurationPrivateFunctionsShould(): reader(), prv()
    {
    }

    void goTo(const std::string& name)
    {
        while(reader.name().toString().toStdString() != name)
            reader.readNext();
    }

    void load(const std::string& str, const std::string& key)
    {
        reader.addData(str.c_str());
        goTo(key);
    }

    QXmlStreamReader reader;
    PublicWrapper prv;
};


TEST_F(DefaultConfigurationPrivateFunctionsShould, ParseEmptyKeysSectionWithoutError)
{
    load("<keys></keys>", "keys");

    const bool status = prv.parseXml_Keys(&reader);

    ASSERT_EQ(true, status);
}


TEST_F(DefaultConfigurationPrivateFunctionsShould, LoadProvidedKey)
{
    load("<keys>"
         "    <key name='1'/>"
         "</keys>",
         "keys"
        );

    const bool status = prv.parseXml_Keys(&reader);
    ASSERT_EQ(true, status);

    ASSERT_EQ(1, prv.getKnownKeys().size());      //one introduced key
    ASSERT_EQ(0, prv.getData().size());           //no data
}


TEST_F(DefaultConfigurationPrivateFunctionsShould, LoadProvidedKeys)
{
    load("<keys>"
         "    <key name='1'/>"
         "    <key name='2'/>"
         "    <key name='3'/>"
         "</keys>",
         "keys"
        );

    const bool status = prv.parseXml_Keys(&reader);
    ASSERT_EQ(true, status);

    ASSERT_EQ(3, prv.getKnownKeys().size());      //three introduced keys
    ASSERT_EQ(0, prv.getData().size());           //no data


}


TEST_F(DefaultConfigurationPrivateFunctionsShould, LoadProvidedDefault)
{
    load("<defaults>"
         "    <key name='1' value='2'/>"
         "</defaults>",
         "defaults"
        );

    const bool status = prv.parseXml_DefaultKeys(&reader);
    ASSERT_EQ(true, status);

    ASSERT_EQ(1, prv.getKnownKeys().size());      //one introduced key
    ASSERT_EQ(1, prv.getData().size());           //no data

    auto data = prv.getData();
    ASSERT_EQ(Configuration2::EntryData("1", "2"), data["1"]); //data for key '1' should be equal to '1':'2'
}


TEST_F(DefaultConfigurationPrivateFunctionsShould, LoadProvidedDefaults)
{
    load("<defaults>"
         "    <key name='1' value='2'/>"
         "    <key name='2' value='4'/>"
         "    <key name='3' value='6'/>"
         "</defaults>",
         "defaults"
        );

    const bool status = prv.parseXml_DefaultKeys(&reader);
    ASSERT_EQ(true, status);

    ASSERT_EQ(3, prv.getKnownKeys().size());      //three introduced keys
    ASSERT_EQ(3, prv.getData().size());           //no data

    auto data = prv.getData();
    ASSERT_EQ(Configuration2::EntryData("1", "2"), data["1"]);
    ASSERT_EQ(Configuration2::EntryData("2", "4"), data["2"]);
    ASSERT_EQ(Configuration2::EntryData("3", "6"), data["3"]);
}

