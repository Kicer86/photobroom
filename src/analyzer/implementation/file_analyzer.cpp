
#include "file_analyzer.hpp"

#include <vector>

#include "ianalyzer.hpp"


struct FileAnalyzer::Impl
{
    Impl(): m_analyzers()
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
    bool status = false;

    for (auto analyzer: m_impl->m_analyzers)
    {
        status = analyzer->isImage(path);

        if (status)
            break;
    }

    return status;
}


void FileAnalyzer::registerAnalyzer(IAnalyzer *analyzer)
{
    std::shared_ptr<IAnalyzer> analyzer_ptr(analyzer);

    m_impl->m_analyzers.push_back(analyzer_ptr);
}
