
#include <cassert>
#include <utility>

#include <QFileInfo>

#include "filesystem_localfile.hpp"


namespace Filesystem
{
    LocalFile::LocalFile(QString path)
        : m_file(QFileInfo(std::move(path)).absoluteFilePath())
    {

    }


    LocalFile::~LocalFile()
    {
        close();
    }


    QByteArrayView LocalFile::asQArrayView() const
    {
        openView();

        return QByteArrayView(
            reinterpret_cast<const char*>(m_data),
            static_cast<qsizetype>(m_size)
        );
    }


    std::span<const std::uint8_t> LocalFile::byteView() const
    {
        openView();

        return std::span<const std::uint8_t>(
            reinterpret_cast<const std::uint8_t*>(m_data),
            static_cast<std::size_t>(m_size)
        );
    }


    bool LocalFile::open(QIODeviceBase::OpenMode mode)
    {
        assert(mode == QIODeviceBase::ReadOnly);

        if (mode != QIODeviceBase::ReadOnly || !m_file.open(mode))
            return false;

        if (QIODevice::open(mode))
            return true;

        m_file.close();
        return false;
    }


    void LocalFile::close()
    {
        if (m_data != nullptr)
        {
            m_file.unmap(m_data);
            m_data = nullptr;
        }

        m_size = 0;
        QIODevice::close();
        m_file.close();
    }


    bool LocalFile::isSequential() const
    {
        return m_file.isSequential();
    }


    qint64 LocalFile::size() const
    {
        return m_file.size();
    }


    bool LocalFile::seek(qint64 position)
    {
        const qint64 oldPosition = m_file.pos();
        if (!m_file.seek(position))
            return false;

        if (QIODevice::seek(position))
            return true;

        m_file.seek(oldPosition);
        return false;
    }


    qint64 LocalFile::readData(char* data, qint64 maxSize)
    {
        return m_file.read(data, maxSize);
    }


    qint64 LocalFile::writeData(const char*, qint64)
    {
        return -1;
    }


    void LocalFile::openView() const
    {
        if (not m_data)
        {
            if (m_file.openMode() == QIODeviceBase::NotOpen)
                return;

            m_size = m_file.size();
            if (m_size == 0)
                return;

            m_data = m_file.map(0, m_size);
            if (m_data == nullptr)
                m_size = 0;
        }
    }
}
