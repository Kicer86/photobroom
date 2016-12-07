
#ifndef TAG_FEEDER_HPP
#define TAG_FEEDER_HPP

#include <memory>
#include <string>

#include <exiv2/exiv2.hpp>

#include "aexif_reader.hpp"

class Exiv2TagFeeder: public AExifReader
{
    public:
        Exiv2TagFeeder(IPhotosManager *);

    private:
        virtual void collect(const QByteArray&);
        virtual std::string read(TagTypes);

        Exiv2::Image::AutoPtr m_exif_data;
};

#endif
