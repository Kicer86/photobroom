
#ifndef ANALYZER_FILE_ANALYZER
#define ANALYZER_FILE_ANALYZER

#include <memory>

#include "ianalyzer.hpp"

class FileAnalyzer: public IAnalyzer
{
    public:
        FileAnalyzer();
        virtual ~FileAnalyzer();

        virtual bool isMediaFile(const QString &) override;
        void registerAnalyzer(std::unique_ptr<IAnalyzer> &&);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

#endif
