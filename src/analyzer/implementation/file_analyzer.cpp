
#include "file_analyzer.hpp"

#include <vector>

#include "ianalyzer.hpp"


struct FileAnalyzer::Impl
{
    Impl() 
    {
    }
    
    ~Impl()
    {
    }    
    
    std::vector<std::shared_ptr<IAnalyzer>> m_analyzers;
};


FileAnalyzer::FileAnalyzer(): m_impl(new Impl)
{

}


FileAnalyzer::~FileAnalyzer()
{
}


bool FileAnalyzer::isImage(const std::string &path)
{
	return false;
}


void FileAnalyzer::registerAnalyzer(IAnalyzer *analyzer)
{
    std::shared_ptr<IAnalyzer> analyzer_ptr(analyzer);
    
    m_impl->m_analyzers.push_back(analyzer_ptr);
}
