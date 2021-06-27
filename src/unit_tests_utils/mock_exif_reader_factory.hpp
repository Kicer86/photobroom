
#ifndef MOCK_EXIF_READER_FACTORY_HPP_INCLUDED
#define MOCK_EXIF_READER_FACTORY_HPP_INCLUDED


#include <core/iexif_reader.hpp>


class ExifReaderFactoryMock: public IExifReaderFactory
{
    public:
        MOCK_METHOD(IExifReader&, get, (), (override));
};

#endif
