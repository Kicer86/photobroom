
#ifndef ANALYZER_FILE_ANALYZER
#define ANALYZER_FILE_ANALYZER

#include "ifile_analyzer.hpp"

class FileAnalyzer: public IFileAnalyzer
{
	public:
		FileAnalyzer();
		virtual ~FileAnalyzer();

		virtual bool isImage(const std::string &);
};

#endif
