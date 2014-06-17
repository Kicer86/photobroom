
#include "easy_exif_tag_feeder.hpp"

#include <assert.h>

#include <fstream>
#include <sstream>

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
	std::ifstream file;
	file.open(path, std::ios_base::in | std::ios_base::binary);

	std::stringstream data;
	file >> data.rdbuf();

	EXIFInfo result;
	result.parseFrom(data.str());

	(void)result;
}

