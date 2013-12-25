
#include "xml_reader.hpp"

#include <QSettings>
#include <QXmlStreamReader>

namespace
{

    bool readXmlFile( QIODevice& device, QSettings::SettingsMap& map )
    {
        QXmlStreamReader xmlReader( &device );

        QString currentElementName;
        while( !xmlReader.atEnd() )
        {
            xmlReader.readNext();
            while( xmlReader.isStartElement() )
            {
                if( xmlReader.name() == "SettingsMap" )
                {
                    xmlReader.readNext();
                    continue;
                }

                if( !currentElementName.isEmpty() )
                {
                    currentElementName += "/";
                }
                currentElementName += xmlReader.name().toString();
                xmlReader.readNext();
            }

            if( xmlReader.isEndElement() )
            {
                continue;
            }

            if( xmlReader.isCharacters() && !xmlReader.isWhitespace() )
            {
                QString key = currentElementName;
                QString value = xmlReader.text().toString();

                map[ key ] = value;

                currentElementName.clear();
            }
        }

        if( xmlReader.hasError() )
        {
            return false;
        }

        return true;
    }

    bool writeXmlFile( QIODevice& device, const QSettings::SettingsMap& map )
    {
        QXmlStreamWriter xmlWriter( &device );
        xmlWriter.setAutoFormatting( true );

        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement( "SettingsMap" );

        QSettings::SettingsMap::const_iterator mi = map.begin();
        for( mi; mi != map.end(); ++mi )
        {
            QStringList groups = mi.key().split("/");
            for(const QString& groupName: groups )
                xmlWriter.writeStartElement(groupName);

            xmlWriter.writeCharacters( mi.value().toString() );

            for(const QString& groupName: groups)
                xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();

        return true;
    }

}


XmlReader::XmlReader() {}

XmlReader::~XmlReader() {}

std::unique_ptr<QSettings> XmlReader::readSettings(const QString& path)
{
    const QSettings::Format XmlFormat = QSettings::registerFormat("xml", readXmlFile, writeXmlFile);

    QSettings* settings = new QSettings(path, XmlFormat);

    return std::unique_ptr<QSettings>(settings);
}




