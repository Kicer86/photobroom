
#include "libav_toolkit.hpp"


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
    m_file = Filesystem::openFile(m_location);

    if (m_formatContext == nullptr || m_ioContext == nullptr)
        return nullptr;

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
    const auto data = m_file->byteView();

    if (m_position >= data.size())
        return AVERROR_EOF;

    const auto remaining = data.size() - m_position;
    const auto amount = std::min<std::size_t>(remaining, static_cast<std::size_t>(bufferSize));
    std::copy_n(data.begin() + m_position, amount, buffer);
    m_position += amount;

    return static_cast<int>(amount);
}


int64_t FileAvio::seek(void* opaque, int64_t offset, int whence)
{
    return static_cast<FileAvio*>(opaque)->seekImpl(offset, whence);
}


int64_t FileAvio::seekImpl(int64_t offset, int whence)
{
    const auto data = m_file->byteView();

    if ((whence & AVSEEK_SIZE) != 0)
    {
        if (data.size() > static_cast<std::size_t>(std::numeric_limits<int64_t>::max()))
            return AVERROR(EINVAL);

        return static_cast<int64_t>(data.size());
    }

    whence &= ~AVSEEK_FORCE;

    int64_t base = 0;
    switch (whence)
    {
        case SEEK_SET:
            base = 0;
            break;
        case SEEK_CUR:
            if (m_position > static_cast<std::size_t>(std::numeric_limits<int64_t>::max()))
                return AVERROR(EINVAL);
            base = static_cast<int64_t>(m_position);
            break;
        case SEEK_END:
            if (data.size() > static_cast<std::size_t>(std::numeric_limits<int64_t>::max()))
                return AVERROR(EINVAL);
            base = static_cast<int64_t>(data.size());
            break;
        default:
            return AVERROR(EINVAL);
    }

    if ((offset > 0 && base > std::numeric_limits<int64_t>::max() - offset) ||
        (offset < 0 && base < std::numeric_limits<int64_t>::min() - offset))
        return AVERROR(EINVAL);

    const int64_t position = base + offset;
    if (position < 0 || static_cast<uint64_t>(position) > data.size())
        return AVERROR(EINVAL);

    m_position = static_cast<std::size_t>(position);
    return position;
}
