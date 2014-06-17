
#include "itagfeeder.hpp"

#if defined USE_EXIV
#include "exif_tag_feeder.hpp"
#elif defined USE_EASY_EXIF
#include "easy_exif_tag_feeder.hpp"
#endif
#include "tag.hpp"


struct NullFeeder: public ITagFeeder
{
    virtual std::unique_ptr<ITagData> getTagsFor(const std::string &) final
    {
        //return empty set
        return std::unique_ptr<ITagData>(new TagData);
    }

    virtual void update(ITagData *, const std::string &) final
    {

    }
};


std::shared_ptr<ITagFeeder> TagFeederFactory::get()
{
    static std::shared_ptr<ITagFeeder> result;

    if (result.get() == nullptr)
#if defined USE_EXIV
        result.reset(new ExifTagFeeder);
#elif defined USE_EASY_EXIF
		result.reset(new EasyExifTagFeeder);
#else
        result.reset(new NullFeeder);
#endif

    return result;
}
