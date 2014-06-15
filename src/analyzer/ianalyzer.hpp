
#ifndef ANALYZER_IANALYZER_HPP
#define ANALYZER_IANALYZER_HPP

#include <string>

struct IAnalyzer
{
    virtual ~IAnalyzer();

    virtual bool isImage(const std::string &) = 0;
};

#endif
