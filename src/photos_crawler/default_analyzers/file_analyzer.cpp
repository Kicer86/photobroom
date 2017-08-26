
#include "file_analyzer.hpp"

#include <core/media_types.hpp>


FileAnalyzer::FileAnalyzer()
{

}


FileAnalyzer::~FileAnalyzer()
{
}


bool FileAnalyzer::isMediaFile(const QString &path)
{
    const bool status = MediaTypes::isImageFile(path) || MediaTypes::isVideoFile(path);

    return status;
}
