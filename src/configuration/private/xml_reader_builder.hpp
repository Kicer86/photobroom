
#ifndef CONFIGURATION_XML_READER_BUILDER_HPP
#define CONFIGURATION_XML_READER_BUILDER_HPP

#include "configuration_export.h"

struct IXmlReader;

class CONFIGURATION_EXPORT ConfigurationBuilder
{
    public:
        ConfigurationBuilder() = delete;
        virtual ~ConfigurationBuilder();

        static ConfigurationBuilder* get();

        IXmlReader* build();
};

#endif
