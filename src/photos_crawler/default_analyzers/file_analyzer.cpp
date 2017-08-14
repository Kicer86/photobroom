
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

    std::vector<std::unique_ptr<IAnalyzer>> m_analyzers;
};


FileAnalyzer::FileAnalyzer(): m_impl(new Impl)
{

}


FileAnalyzer::~FileAnalyzer()
{
}


bool FileAnalyzer::isMediaFile(const QString &path)
{
    bool status = false;

    for (std::unique_ptr<IAnalyzer>& analyzer: m_impl->m_analyzers)
    {
        status = analyzer->isMediaFile(path);

        if (status)
            break;
    }

    return status;
}


void FileAnalyzer::registerAnalyzer(std::unique_ptr<IAnalyzer>&& analyzer)
{
    m_impl->m_analyzers.push_back( std::move(analyzer) );
}
