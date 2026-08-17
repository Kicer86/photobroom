
#ifndef LIBAV_TOOLKIT_HPP_INCLUDED
#define LIBAV_TOOLKIT_HPP_INCLUDED

extern "C"
{
#include <libavformat/avformat.h>
}

#include <memory>

#include "filesystem.hpp"


class FileAvio
{
public:
    explicit FileAvio(const Filesystem::Location& location);
    FileAvio(const FileAvio&) = delete;
    FileAvio& operator=(const FileAvio&) = delete;

    ~FileAvio();

    AVFormatContext* open();

private:
    static int read(void* opaque, unsigned char* buffer, int bufferSize);
    int readImpl(unsigned char* buffer, int bufferSize);
    static int64_t seek(void* opaque, int64_t offset, int whence);
    int64_t seekImpl(int64_t offset, int whence);

    Filesystem::Location m_location;
    std::unique_ptr<Filesystem::IFile> m_file;
    std::size_t m_position = 0;
    AVFormatContext* m_formatContext = nullptr;
    AVIOContext* m_ioContext = nullptr;
};

#endif
