
#ifndef ANALYZER_IFILE_ANALYZER
#define ANALYZER_IFILE_ANALYZER

#include <string>

struct IAnalyzer;

struct IFileAnalyzer
{
	virtual ~IFileAnalyzer () {}

	virtual bool isImage(const std::string &) = 0;
	virtual void registerAnalyzer(IAnalyzer *) = 0;
};

#endif
