
#include "libav_toolkit.hpp"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <limits>


FileAvio::FileAvio(const Filesystem::Location& location)
    : m_location(location)
{
    m_formatContext = avformat_alloc_context();
    if (m_formatContext == nullptr)
        return;

    constexpr int bufferSize = 4096;
    auto* buffer = static_cast<unsigned char*>(av_malloc(bufferSize));
    if (buffer == nullptr)
        return;

    m_ioContext = avio_alloc_context(
        buffer,
        bufferSize,
        0,
        this,
        &FileAvio::read,
        nullptr,
        &FileAvio::seek
    );

    if (m_ioContext == nullptr)
    {
        av_free(buffer);
        return;
    }

    m_formatContext->pb = m_ioContext;
    m_formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;
}


FileAvio::~FileAvio()
{
    if (m_formatContext != nullptr)
        avformat_close_input(&m_formatContext);

    if (m_ioContext != nullptr)
    {
        av_freep(&m_ioContext->buffer);
        avio_context_free(&m_ioContext);
    }
}


AVFormatContext* FileAvio::open()
{
    assert(not m_file);

    if (m_formatContext == nullptr || m_ioContext == nullptr)
        return nullptr;

    m_file = Filesystem::openFile(m_location);
    if (!m_file || !m_file->isOpen() || !m_file->isReadable())
        return nullptr;

    m_ioContext->seekable = m_file->isSequential() ? 0 : AVIO_SEEKABLE_NORMAL;

    if (avformat_open_input(&m_formatContext, nullptr, nullptr, nullptr) < 0)
        return nullptr;

    return m_formatContext;
}


int FileAvio::read(void* opaque, unsigned char* buffer, int bufferSize)
{
    return static_cast<FileAvio*>(opaque)->readImpl(buffer, bufferSize);
}


int FileAvio::readImpl(unsigned char* buffer, int bufferSize)
{
    if (!m_file || !m_file->isOpen() || !m_file->isReadable() || buffer == nullptr || bufferSize <= 0)
        return AVERROR(EINVAL);

    const qint64 amount = m_file->read(reinterpret_cast<char*>(buffer), bufferSize);
    if (amount > 0)
        return static_cast<int>(amount);

    if (amount == 0)
        return m_file->atEnd() ? AVERROR_EOF : AVERROR(EAGAIN);

    return AVERROR(EIO);
}


int64_t FileAvio::seek(void* opaque, int64_t offset, int whence)
{
    return static_cast<FileAvio*>(opaque)->seekImpl(offset, whence);
}


int64_t FileAvio::seekImpl(int64_t offset, int whence)
{
    if (!m_file || !m_file->isOpen())
        return AVERROR(EIO);

    if ((whence & AVSEEK_SIZE) != 0)
    {
        if (m_file->isSequential())
            return AVERROR(ENOSYS);

        const qint64 size = m_file->size();
        return size >= 0 ? static_cast<int64_t>(size) : AVERROR(EIO);
    }

    if (m_file->isSequential())
        return AVERROR(ENOSYS);

    int64_t base = 0;
    switch (whence & ~(AVSEEK_SIZE | AVSEEK_FORCE))
    {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            base = m_file->pos();
            break;
        case SEEK_END:
            base = m_file->size();
            break;
        default:
            return AVERROR(EINVAL);
    }

    if (base < 0 ||
        (offset > 0 && base > std::numeric_limits<int64_t>::max() - offset) ||
        (offset < 0 && base < std::numeric_limits<int64_t>::min() - offset))
        return AVERROR(EINVAL);

    const int64_t target = base + offset;
    if (target < 0)
        return AVERROR(EINVAL);

    if (!m_file->seek(static_cast<qint64>(target)))
        return AVERROR(EIO);

    const qint64 position = m_file->pos();
    return position >= 0 ? static_cast<int64_t>(position) : AVERROR(EIO);
}
