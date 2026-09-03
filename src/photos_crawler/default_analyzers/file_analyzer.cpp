
#include "file_analyzer.hpp"

#include <core/media_types.hpp>


FileAnalyzer::FileAnalyzer()
{

}


FileAnalyzer::~FileAnalyzer()
{
}


bool FileAnalyzer::isMediaFile(const Filesystem::Location& location)
{
    const bool status = MediaTypes::isImageFile(location) || MediaTypes::isVideoFile(location);

    return status;
}
