
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <memory>
#include <string>

class QString;

struct TagDataBase;

struct ITagFeeder
{
    virtual ~ITagFeeder() {}

    virtual std::unique_ptr<TagDataBase> getTagsFor(const QString& path) = 0;
    virtual void update(TagDataBase *, const QString& path) = 0;
};

struct ITagFeederFactory
{
	virtual ~ITagFeederFactory() {}

	virtual std::shared_ptr<ITagFeeder> get() = 0;
};

#endif
