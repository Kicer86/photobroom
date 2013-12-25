
#ifndef CONFIGURATION_XML_READER_BUILDER_HPP
#define CONFIGURATION_XML_READER_BUILDER_HPP

#include "configuration_export.h"

struct IXmlReader;

class CONFIGURATION_EXPORT XmlReaderBuilder
{
    public:
        virtual ~XmlReaderBuilder();

        static XmlReaderBuilder* get();

        IXmlReader* build();

    private:
        XmlReaderBuilder();
};

#endif
