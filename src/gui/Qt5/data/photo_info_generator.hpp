
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <core/photo_info.hpp>


class PhotoInfoGenerator final
{
    public:
        PhotoInfoGenerator();
        ~PhotoInfoGenerator();

        PhotoInfoGenerator(const PhotoInfoGenerator &) = delete;
        PhotoInfoGenerator& operator=(const PhotoInfoGenerator &) = delete;

        PhotoInfo::Ptr get(const std::string& path);
};

#endif
