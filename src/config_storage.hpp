
#ifndef CONFIG_STORAGE_HPP_INCLUDED
#define CONFIG_STORAGE_HPP_INCLUDED

#include <QString>

#include "core/configuration.hpp"


class ConfigStorage: public IConfigStorage
{
public:
    explicit ConfigStorage(const QString& configFile);

    IConfigStorage::Content load() override;
    void save(const IConfigStorage::Content& configuration) override;

private:
    QString m_configFile;

    void writeTo(QJsonObject& obj, QStringList configPath, const QVariant& value);
    IConfigStorage::Content readNode(const QJsonObject& obj, const QString& entry_namespace = {});
};

#endif
