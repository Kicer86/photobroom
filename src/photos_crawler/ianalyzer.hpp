
#ifndef ANALYZER_IANALYZER_HPP
#define ANALYZER_IANALYZER_HPP

#include <core/filesystem.hpp>
#include "photos_crawler_export.h"


struct PHOTOS_CRAWLER_EXPORT IAnalyzer
{
    virtual ~IAnalyzer() = default;

    virtual bool isMediaFile(const Filesystem::Location &) = 0;
};

#endif
