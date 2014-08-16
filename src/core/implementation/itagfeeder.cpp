
#include "itagfeeder.hpp"

#if defined USE_EXIV2
#include "exif_tag_feeder.hpp"
#elif defined USE_EASY_EXIF
#include "easy_exif_tag_feeder.hpp"
#endif
#include "tag.hpp"


struct NullFeeder: public ITagFeeder
{
        virtual ~NullFeeder() {}

    private:
        virtual std::unique_ptr<ITagData> getTagsFor(const QString &) override final
        {
            //return empty set
            return std::unique_ptr<ITagData>(new TagData);
        }

        virtual void update(ITagData *, const QString &) override final
        {

        }
};


std::shared_ptr<ITagFeeder> TagFeederFactory::get()
{
    static std::shared_ptr<ITagFeeder> result;

    if (result.get() == nullptr)
#if defined USE_EXIV2
        result.reset(new Exiv2TagFeeder);
#elif defined USE_EASY_EXIF
	result.reset(new EasyExifTagFeeder);
#else
        result.reset(new NullFeeder);
#endif

    return result;
}
