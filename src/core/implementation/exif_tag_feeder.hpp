
#ifndef TAG_FEEDER_HPP
#define TAG_FEEDER_HPP

#include <memory>
#include <string>

#include <exiv2/exiv2.hpp>

#include "atagfeeder.hpp"

class ExifTagFeeder: public ATagFeeder
{
    public:
        ExifTagFeeder();

    private:
        virtual void collect(const QByteArray&);
        virtual std::string get(TagTypes);

        Exiv2::Image::AutoPtr m_exif_data;
};

#endif
