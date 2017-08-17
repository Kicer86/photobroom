
#ifndef EXIV2_EXIF_READER_HPP
#define EXIV2_EXIF_READER_HPP

#include <memory>
#include <string>

#include <exiv2/exiv2.hpp>

#include "aexif_reader.hpp"

class Exiv2ExifReader: public AExifReader
{
    public:
        Exiv2ExifReader();
        Exiv2ExifReader(const Exiv2ExifReader &) = delete;
        Exiv2ExifReader(Exiv2ExifReader &&) = delete;

        Exiv2ExifReader& operator=(const Exiv2ExifReader &) = delete;
        Exiv2ExifReader& operator=(Exiv2ExifReader &&) = delete;

    private:
        virtual void collect(const QString &) override;
        virtual std::string read(TagType) const override;

        Exiv2::Image::AutoPtr m_exif_data;
};

#endif
