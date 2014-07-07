
#ifndef ANALYZER_IPHOTO_CRAWLER_HPP
#define ANALYZER_IPHOTO_CRAWLER_HPP

#include <vector>

#include "analyzer_export.h"

class QString;

struct Rules
{

};


struct IMediaNotification
{
    virtual void found(const QString &) = 0;
};

struct ANALYZER_EXPORT IPhotoCrawler
{
    virtual ~IPhotoCrawler();

    virtual void crawl(const QString &, IMediaNotification *) = 0;   //find media files for given path. Notify about each result
    virtual void setRules(const Rules &) = 0;                        //provide crawl rules
};

#endif
