
#ifndef ANALYZER_FILESYSTEM_SCANNER_HPP
#define ANALYZER_FILESYSTEM_SCANNER_HPP

#include <vector>
#include <string>

struct IFileSystemScanner
{
    virtual ~IFileSystemScanner();
    
    virtual std::vector<std::string> getFilesFor(const std::string &) = 0;
};

#endif
