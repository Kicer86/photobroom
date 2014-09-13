
#ifndef CONFIGURATION_DEFAULT_CONFIGURATION_PRIVATE_HPP
#define CONFIGURATION_DEFAULT_CONFIGURATION_PRIVATE_HPP

#include <unordered_map>
#include <unordered_set>

#include <OpenLibrary/utils/optional.hpp>

#include "entrydata.hpp"


class QXmlStreamReader;

namespace Configuration
{
    struct IInitializer;
}

struct DefaultConfigurationPrivate
{
        DefaultConfigurationPrivate();
        virtual ~DefaultConfigurationPrivate();

        QString getConfigDir() const;
        Optional<Configuration::EntryData> find(const Configuration::ConfigurationKey& key) const;
        std::vector<Configuration::EntryData> getAll() const;
        void addEntry(const Configuration::ConfigurationKey& key, const Configuration::EntryData& data, bool def = false);
        void introduceKey(const Configuration::ConfigurationKey& key);
        bool useXml(const QString &xml);
        bool load();

    protected:
        struct hash
        {
            std::size_t operator()(const Configuration::ConfigurationKey& s) const
            {
                return std::hash<std::string>()(s.getKeyRaw());
            }
        };

        std::unordered_set<Configuration::ConfigurationKey, hash> m_known_keys;
        std::unordered_map<Configuration::ConfigurationKey, Configuration::EntryData, hash> m_data;

        bool parseXml_Keys(QXmlStreamReader* reader);
        bool parseXml_DefaultKeys(QXmlStreamReader* reader);
        bool gotoNextUseful(QXmlStreamReader* reader);
        void verifyKey(const Configuration::ConfigurationKey& key) const;
};

#endif
