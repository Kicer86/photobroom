
#ifndef CONFIGURATION_XML_READER_HPP
#define CONFIGURATION_XML_READER_HPP

#include "ixml_reader.hpp"

class XmlReader: public IXmlReader
{
    public:
        XmlReader();
        virtual ~XmlReader();

        virtual std::unique_ptr<QSettings> readSettings(const QString &) override;
};

#endif
