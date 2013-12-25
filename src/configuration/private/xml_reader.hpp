
#ifndef CONFIGURATION_XML_READER_HPP
#define CONFIGURATION_XML_READER_HPP

#include "ixml_reader.hpp"

class XmlReader: public IXmlReader
{
    public:
        XmlReader();
        virtual ~XmlReader();

        virtual QSettings* getSettings() override;
};

#endif
