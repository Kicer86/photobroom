
#include "tag_feeder_factory.hpp"

#include <thread>

#if defined USE_EXIV2
	#include "exiv2_tag_feeder.hpp"
	#define ExifTool Exiv2TagFeeder
#elif defined USE_EASY_EXIF
	#include "easy_exif_tag_feeder.hpp"
	#define ExifTool EasyExifTagFeeder
#else
	#define ExifTool NullFeeder
#endif

#include "tag.hpp"
#include "itagfeeder.hpp"

struct NullFeeder: public ITagFeeder
{
	virtual ~NullFeeder() {}

private:
    virtual Tag::TagsList getTagsFor(const QString &) override final
	{
		//return empty set
        return Tag::TagsList();
	}

};


TagFeederFactory::TagFeederFactory()
{
	
}


std::shared_ptr<ITagFeeder> TagFeederFactory::get()
{
	//ExifTool may not be thread safe. Prepare separate object for each thread
	const auto id = std::this_thread::get_id();
	auto it = m_feeders.find(id);

	if (it == m_feeders.end())
	{
		auto feeder = std::make_shared<ExifTool>();
		auto data = std::make_pair(id, feeder);
		auto in = m_feeders.insert(data);

		it = in.first;
	}

	return it->second;;
}
