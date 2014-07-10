
#ifndef ANALYZER_IANALYZER_HPP
#define ANALYZER_IANALYZER_HPP

class QString;

struct IAnalyzer
{
    virtual ~IAnalyzer();

    virtual bool isImage(const QString &) = 0;
};

#endif
