
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <core/aphoto_info.hpp>


class PhotoInfoGenerator
{
    public:
        PhotoInfoGenerator();
        virtual ~PhotoInfoGenerator();

        PhotoInfoGenerator(const PhotoInfoGenerator &) = delete;
        PhotoInfoGenerator& operator=(const PhotoInfoGenerator &) = delete;

        APhotoInfo::Ptr get(const std::string& path);
};


#endif
