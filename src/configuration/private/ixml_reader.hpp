
#ifndef CONFIGURATION_IXML_READER_HPP
#define CONFIGURATION_IXML_READER_HPP

class QSettings;

struct IXmlReader
{
    virtual ~IXmlReader() {}

    virtual QSettings* getSettings() = 0;
};

#endif
