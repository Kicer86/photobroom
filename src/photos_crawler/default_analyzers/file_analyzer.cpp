
#include "file_analyzer.hpp"

#include <core/media_types.hpp>


FileAnalyzer::FileAnalyzer()
{

}


FileAnalyzer::~FileAnalyzer()
{
}


bool FileAnalyzer::isMediaFile(const QString& filename)
{
    const bool status = MediaTypes::isImageFile(filename) || MediaTypes::isVideoFile(filename);

    return status;
}
