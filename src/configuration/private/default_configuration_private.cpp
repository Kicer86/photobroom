

#include "default_configuration_private.hpp"

#include <iostream>

#include <QXmlStreamReader>
#include <QFile>

#include "system/system.hpp"

#include "iconfiguration.hpp"


DefaultConfigurationPrivate::DefaultConfigurationPrivate() : m_known_keys(), m_data(), m_initializers()
{

}


DefaultConfigurationPrivate::~DefaultConfigurationPrivate()
{

}


std::string DefaultConfigurationPrivate::getConfigDir() const
{
    const std::string result = System::getApplicationConfigDir();

    return result;
}


boost::optional<Configuration::EntryData> DefaultConfigurationPrivate::find(const Configuration::ConfigurationKey& key) const
{
    verifyKey(key);

    boost::optional<Configuration::EntryData> result;
    auto it = m_data.find(key);

    if (it != m_data.end())
        result = it->second;

    return result;
}


std::vector<Configuration::EntryData> DefaultConfigurationPrivate::getAll() const
{
    std::vector<Configuration::EntryData> result;

    for(const auto & it : m_data)
        result.push_back(it.second);

    return result;
}


void DefaultConfigurationPrivate::addEntry(const Configuration::ConfigurationKey& key, const Configuration::EntryData& data, bool def)
{
    verifyKey(key);

    //for default entries
    //add only those which are not present yet
    bool add = def ? m_data.find(key) == m_data.end() : true;

    if (add)
        m_data[key] = data;
}


void DefaultConfigurationPrivate::introduceKey(const Configuration::ConfigurationKey& key)
{
    m_known_keys.insert(key);
}


bool DefaultConfigurationPrivate::useXml(const QString& xml)
{
    QXmlStreamReader reader(xml);
    int level = 0;
    bool status = true;

    while(status && reader.atEnd() == false)
    {
        if (reader.isStartElement())
        {
            level++;

            const QStringRef name = reader.name();

            if (level == 1 && name == "configuration")
            {
                //just do nothing
            }
            else
                if (level == 2 && name == "keys")
                    status = parseXml_Keys(&reader);
                else
                    if (level == 2 && name == "defaults")
                        status = parseXml_DefaultKeys(&reader);
                    else
                    {
                        std::cerr << "DefaultConfiguration: invalid format of xml file (unknown tag: "
                                  << name.toString().toStdString()
                                  << ")"
                                  << std::endl;

                        status = false;
                        break;
                    }
        }

        if (reader.isEndElement())
            level--;

        reader.readNext();
    }

    return status;
}


void DefaultConfigurationPrivate::registerInitializer(Configuration::IInitializer* i)
{
    m_initializers.push_back(i);
}


bool DefaultConfigurationPrivate::load()
{
    bool status = true;

    for(Configuration::IInitializer* i: m_initializers)
    {
        const std::string xml = i->getXml();
        status = useXml(xml.c_str());

        if (!status)
            break;
    }

    return status;
}


bool DefaultConfigurationPrivate::parseXml_Keys(QXmlStreamReader* reader)
{
    bool status = true;

    status = gotoNextUseful(reader);

    while (status && reader->tokenType() == QXmlStreamReader::StartElement)       //start element should came
    {
        const QStringRef token = reader->name();

        if (token == "key")
        {
            const QXmlStreamAttributes attrs = reader->attributes();
            const QStringRef name = attrs.value("name");

            if (name.isEmpty() == false)
            {
                Configuration::ConfigurationKey key(name.toString());
                introduceKey(key);
            }
            else
                std::cerr << "DefaultConfiguration: in <keys> section there is a key with null name" << std::endl;
        }
        else
        {
            status = false;
            break;
        }

        status = gotoNextUseful(reader);

        if (status == false)
            break;

        //now we expect end element for <key>
        const QXmlStreamReader::TokenType type = reader->tokenType();

        if (type != QXmlStreamReader::EndElement)
        {
            status = false;
            break;
        }

        //let's move to next element
        status = gotoNextUseful(reader);
    }

    //here we expect end element for <keys>
    const QXmlStreamReader::TokenType type = reader->tokenType();

    if (type != QXmlStreamReader::EndElement)
        status = false;

    return status;
}


bool DefaultConfigurationPrivate::parseXml_DefaultKeys(QXmlStreamReader* reader)
{
    bool status = true;

    status = gotoNextUseful(reader);

    while (status && reader->tokenType() == QXmlStreamReader::StartElement)       //start element should came
    {
        const QStringRef token = reader->name();

        if (token == "key")
        {
            const QXmlStreamAttributes attrs = reader->attributes();
            const QStringRef name = attrs.value("name");
            const QStringRef value = attrs.value("value");

            if (name.isEmpty() == false)
            {
                //introduce key
                Configuration::ConfigurationKey key(name.toString());
                introduceKey(key);

                //set default value
                Configuration::EntryData entry(key, value.toString().toStdString());
                addEntry(key, entry);
            }
            else
                std::cerr << "DefaultConfiguration: in <defaults> section there is a key with null name" << std::endl;
        }
        else
        {
            status = false;
            break;
        }

        status = gotoNextUseful(reader);

        if (status == false)
            break;

        const QXmlStreamReader::TokenType type = reader->tokenType();   //now we expect end element

        if (type != QXmlStreamReader::EndElement)
        {
            status = false;
            break;
        }

        //let's move to next element
        status = gotoNextUseful(reader);
    }

    //here we expect end element for <keys>
    const QXmlStreamReader::TokenType type = reader->tokenType();

    if (type != QXmlStreamReader::EndElement)
        status = false;

    return status;
}


bool DefaultConfigurationPrivate::gotoNextUseful(QXmlStreamReader* reader)
{
    QXmlStreamReader::TokenType type = QXmlStreamReader::Invalid;

    do
    {
        type = reader->readNext();
    }
    while(type == QXmlStreamReader::Comment || type == QXmlStreamReader::Characters);

    const bool status = reader->hasError() == false &&
                        type != QXmlStreamReader::Invalid &&
                        type != QXmlStreamReader::NoToken;

    return status;
}


void DefaultConfigurationPrivate::verifyKey(const Configuration::ConfigurationKey& key) const
{
    if (m_known_keys.find(key) == m_known_keys.end())
        std::cerr << "DefaultConfiguration: unknown key: " << key.getKeyRaw() << std::endl;
}
