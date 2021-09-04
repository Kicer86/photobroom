
#ifndef ANALYZER_FILE_ANALYZER
#define ANALYZER_FILE_ANALYZER

#include "ianalyzer.hpp"

class FileAnalyzer final: public IAnalyzer
{
    public:
        FileAnalyzer();
        virtual ~FileAnalyzer();

        virtual bool isMediaFile(const QString &) override;
};

#endif
