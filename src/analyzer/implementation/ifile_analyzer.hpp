
#ifndef ANALYZER_IFILE_ANALYZER
#define ANALYZER_IFILE_ANALYZER

#include <string>

struct IFileAnalyzer
{
	virtual ~IFileAnalyzer () {}
	virtual bool isImage(const std::string &) = 0;
};

#endif
