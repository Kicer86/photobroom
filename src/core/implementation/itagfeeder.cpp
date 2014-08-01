
#include "itagfeeder.hpp"

#ifdef USE_EXIV
#include "exif_tag_feeder.hpp"
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
#ifdef USE_EXIV
        result.reset(new ExifTagFeeder);
#else
        result.reset(new NullFeeder);
#endif

    return result;
}
