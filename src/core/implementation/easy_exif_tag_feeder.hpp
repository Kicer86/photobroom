
#ifndef EASY_EXIF_TAG_FEEDER_HPP
#define EASY_EXIF_TAG_FEEDER_HPP

#include <memory>
#include <string>

#include <easyexif/exif.h>

#include "core_export.h"
#include "atagfeeder.hpp"

class EasyExifTagFeeder: public ATagFeeder
{
    public:
        EasyExifTagFeeder(IPhotosManager *);

    private:
        EXIFInfo m_exif_data;

        void collect(const QByteArray &) override;
        std::string get(TagTypes) override;
};

#endif

