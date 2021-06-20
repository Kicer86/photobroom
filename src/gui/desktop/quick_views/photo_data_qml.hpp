
#ifndef PHOTODATAQML_HPP
#define PHOTODATAQML_HPP

#include <database/photo_data.hpp>

/**
 * wrapper for Photo::Data which provides Qml compatible access to Photo::Data members
 */
class PhotoDataQml
{
    public:
        PhotoDataQml(const Photo::Data &);
};

#endif
