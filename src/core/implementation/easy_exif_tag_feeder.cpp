
#include "easy_exif_tag_feeder.hpp"

#include <assert.h>

#include <QStringList>
#include <easyexif/exif.h>

#include <configuration/constants.hpp>

#include "base_tags.hpp"


EasyExifTagFeeder::EasyExifTagFeeder()
{

}


std::unique_ptr<ITagData> EasyExifTagFeeder::getTagsFor(const std::string &path)
{
	std::unique_ptr<ITagData> tagData(new TagData);
	feed(path, tagData.get());

	return tagData;
}


void EasyExifTagFeeder::update(ITagData *, const std::string &)
{

}


void EasyExifTagFeeder::feed(const std::string& path, ITagData* tagData)
{
	
}

