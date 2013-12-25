
#ifndef CONFIGURATION_IXML_READER_HPP
#define CONFIGURATION_IXML_READER_HPP

#include <memory>

class QSettings;
class QString;

struct IXmlReader
{
    virtual ~IXmlReader() {}

    virtual std::unique_ptr<QSettings> readSettings(const QString &) = 0;
};

#endif
