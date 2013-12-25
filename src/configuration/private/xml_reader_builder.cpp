
#include "xml_reader_builder.hpp"

#include "xml_reader.hpp"


XmlReaderBuilder::XmlReaderBuilder() {}

XmlReaderBuilder::~XmlReaderBuilder() {}

XmlReaderBuilder* XmlReaderBuilder::get()
{
    static XmlReaderBuilder builder;

    return &builder;
}


IXmlReader* XmlReaderBuilder::build()
{
    static XmlReader reader;

    return &reader;
}
