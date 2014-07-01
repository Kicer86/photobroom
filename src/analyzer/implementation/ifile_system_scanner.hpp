
#ifndef ANALYZER_FILESYSTEM_SCANNER_HPP
#define ANALYZER_FILESYSTEM_SCANNER_HPP

#include <vector>
#include <string>

struct IFileNotifier
{
    virtual ~IFileNotifier() {}

    virtual void found(const std::string &) = 0;
};

struct IFileSystemScanner
{
    virtual ~IFileSystemScanner();

    virtual void getFilesFor(const std::string &, IFileNotifier *) = 0;
};

#endif
