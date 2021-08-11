
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSaveFile>

#include "config_storage.hpp"


ConfigStorage::ConfigStorage(const QString& configFile)
    : m_configFile(configFile)
{

}


IConfigStorage::Content ConfigStorage::load()
{
    IConfigStorage::Content config;

    if (QFile::exists(m_configFile))
    {
        QFile configFile(m_configFile);
        configFile.open(QIODevice::ReadOnly);

        const QByteArray configFileContent = configFile.readAll();
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(configFileContent);
        const QJsonObject configJsonObj = jsonDoc.object();

        config = readNode(configJsonObj);

    }
    else
    {
        //load default data
    }

    return config;
}


void ConfigStorage::save(const IConfigStorage::Content& configuration)
{
    const QFileInfo configPathInfo(m_configFile);
    const QDir configDir(configPathInfo.absolutePath());

    if (configDir.exists() == false)
        configDir.mkpath(".");

    QJsonObject configurationObject;

    for(const auto& [key, value]: configuration)
    {
        const QStringList entries = key.split("::");
        writeTo(configurationObject, entries, value);
    }

    QJsonDocument jsonDoc(configurationObject);

    QSaveFile configFile(m_configFile);

    configFile.open(QIODevice::WriteOnly);
    configFile.write(jsonDoc.toJson());
    configFile.commit();
}


void ConfigStorage::writeTo(QJsonObject& obj, QStringList configPath, const QVariant& value)
{
    const QString entryName = configPath.front();

    if (configPath.size() == 1)
    {
        const QJsonValue json_value = QJsonValue::fromVariant(value);
        assert(json_value.isUndefined() == false);
        assert(json_value.isNull() == false);

        obj[entryName] = json_value;
    }
    else
    {
        configPath.takeFirst();
        QJsonValueRef jsonValue = obj[entryName];
        assert(jsonValue.isNull() || jsonValue.isObject());

        QJsonObject subObj = jsonValue.toObject();
        writeTo(subObj, configPath, value);

        jsonValue = subObj;
    }
}


IConfigStorage::Content ConfigStorage::readNode(const QJsonObject& obj, const QString& entry_namespace)
{
    IConfigStorage::Content content;

    for(auto it = obj.constBegin(); it != obj.constEnd(); ++it)
    {
        const QString name = it.key();
        const QJsonValue value = it.value();

        assert(value.isArray() == false);
        assert(value.isNull() == false);

        const QString sub_namespace = entry_namespace + (entry_namespace.isEmpty()? name : QString("::%1").arg(name));

        if (value.isObject())
        {
            const QJsonObject sub_obj = value.toObject();
            const IConfigStorage::Content sub_content = readNode(sub_obj, sub_namespace);

            content.insert(sub_content.cbegin(), sub_content.cend());
        }
        else
            content.emplace(sub_namespace, value);
    }

    return content;
}
