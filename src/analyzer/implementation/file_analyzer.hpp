
#ifndef ANALYZER_FILE_ANALYZER
#define ANALYZER_FILE_ANALYZER

#include <memory>

#include "ifile_analyzer.hpp"
#include "ianalyzer.hpp"

class FileAnalyzer: public IAnalyzer, public IFileAnalyzer
{
	public:
		FileAnalyzer();
		virtual ~FileAnalyzer();

		virtual bool isImage(const std::string &) override;
        virtual void registerAnalyzer(IAnalyzer* ) override;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

#endif
