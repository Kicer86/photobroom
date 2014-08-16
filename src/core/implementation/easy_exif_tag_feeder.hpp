
#ifndef EASY_EXIF_TAG_FEEDER_HPP
#define EASY_EXIF_TAG_FEEDER_HPP

#include <memory>
#include <string>

#include "core_export.h"
#include "itagfeeder.hpp"

struct ITagData;

class EasyExifTagFeeder : public ITagFeeder
{
public:
	EasyExifTagFeeder();

	virtual std::unique_ptr<ITagData> getTagsFor(const QString& path) override;
	virtual void update(ITagData *, const QString& path) override;

private:
	void feed(const QString& path, ITagData *);
};

#endif

