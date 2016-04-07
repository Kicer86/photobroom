
#ifndef ANALYZER_FILESYSTEM_SCANNER_HPP
#define ANALYZER_FILESYSTEM_SCANNER_HPP

#include <vector>

#include "photos_crawler_export.h"

class QString;

struct PHOTOS_CRAWLER_EXPORT IFileNotifier
{
    virtual ~IFileNotifier();

    virtual void found(const QString &) = 0;
    virtual void finished() = 0;
};


struct PHOTOS_CRAWLER_EXPORT IFileSystemScanner
{
    virtual ~IFileSystemScanner();

    virtual void getFilesFor(const QString &, IFileNotifier *) = 0;
    virtual void stop() = 0;
};

#endif
