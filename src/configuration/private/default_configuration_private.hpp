
#ifndef CONFIGURATION_DEFAULT_CONFIGURATION_PRIVATE_HPP
#define CONFIGURATION_DEFAULT_CONFIGURATION_PRIVATE_HPP

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <OpenLibrary/utils/optional.hpp>

#include "entrydata.hpp"


class QXmlStreamReader;

namespace Configuration2
{
    struct IInitializer;
}

struct ILoggerFactory;
struct ILogger;

struct DefaultConfigurationPrivate
{
        DefaultConfigurationPrivate();
        virtual ~DefaultConfigurationPrivate();

        DefaultConfigurationPrivate(const DefaultConfigurationPrivate &) = delete;
        DefaultConfigurationPrivate& operator=(const DefaultConfigurationPrivate &) = delete;

        void set(ILoggerFactory *);

        QString getConfigDir() const;
        ol::Optional<Configuration2::EntryData> find(const Configuration2::ConfigurationKey& key) const;
        std::vector<Configuration2::EntryData> getAll() const;
        void addEntry(const Configuration2::ConfigurationKey& key, const Configuration2::EntryData& data, bool def = false);
        void introduceKey(const Configuration2::ConfigurationKey& key);
        bool useXml(const QString &xml);
        bool load();

    protected:
        struct hash
        {
            std::size_t operator()(const Configuration2::ConfigurationKey& s) const
            {
                return std::hash<std::string>()(s.getKeyRaw());
            }
        };

        std::unordered_set<Configuration2::ConfigurationKey, hash> m_known_keys;
        std::unordered_map<Configuration2::ConfigurationKey, Configuration2::EntryData, hash> m_data;
        std::unique_ptr<ILogger> m_logger;

        bool parseXml_Keys(QXmlStreamReader* reader);
        bool parseXml_DefaultKeys(QXmlStreamReader* reader);
        bool gotoNextUseful(QXmlStreamReader* reader);
        void verifyKey(const Configuration2::ConfigurationKey& key) const;
};

#endif

