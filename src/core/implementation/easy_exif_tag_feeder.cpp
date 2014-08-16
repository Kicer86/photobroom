
#include "easy_exif_tag_feeder.hpp"

#include <assert.h>

#include <QByteArray>

#include <easyexif/exif.h>

#include <configuration/constants.hpp>

#include "base_tags.hpp"
#include "photos_manager.hpp"


EasyExifTagFeeder::EasyExifTagFeeder()
{

}


std::unique_ptr<ITagData> EasyExifTagFeeder::getTagsFor(const QString &path)
{
	std::unique_ptr<ITagData> tagData(new TagData);
	feed(path, tagData.get());

	return tagData;
}


void EasyExifTagFeeder::update(ITagData *, const QString &)
{

}


void EasyExifTagFeeder::feed(const QString& path, ITagData* tagData)
{
	QByteArray data;

	PhotosManager::instance()->getPhoto(path, &data);
	
	EXIFInfo result;

	const unsigned char* rawData = reinterpret_cast<const unsigned char *>(data.data());
	const std::size_t rawDataSize = data.size();

	result.parseFrom(rawData, rawDataSize);

	(void)result;
}

