
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <core/photo_info.hpp>


//TODO: construct photo manualy. Add fillers manualy on demand
class __attribute__((deprecated)) PhotoInfoGenerator final
{
    public:
        PhotoInfoGenerator();
        ~PhotoInfoGenerator();

        PhotoInfoGenerator(const PhotoInfoGenerator &) = delete;
        PhotoInfoGenerator& operator=(const PhotoInfoGenerator &) = delete;

        PhotoInfo::Ptr get(const std::string& path);
};

#endif
