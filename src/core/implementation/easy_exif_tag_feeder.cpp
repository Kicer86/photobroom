
#include "easy_exif_tag_feeder.hpp"

#include <assert.h>

#include <QFile>

#include <easyexif/exif.h>

#include <configuration/constants.hpp>

#include "base_tags.hpp"


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
	QFile file(path);
	file.open(QIODevice::ReadOnly);

	const QByteArray data = file.readAll();

	EXIFInfo result;
	result.parseFrom(data.data());
}

