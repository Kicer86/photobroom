
#ifndef EASY_EXIF_TAG_FEEDER_HPP
#define EASY_EXIF_TAG_FEEDER_HPP

#include <memory>
#include <string>

#include <easyexif/exif.h>

#include "core_export.h"
#include "aexif_reader.hpp"

class EasyExifTagFeeder: public AExifReader
{
    public:
        explicit EasyExifTagFeeder(IPhotosManager *);

    private:
        easyexif::EXIFInfo m_exif_data;

        void collect(const QByteArray &) override;
        std::string read(Tag::Types) override;
};

#endif

