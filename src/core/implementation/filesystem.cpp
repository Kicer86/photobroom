
#include <cassert>
#include <utility>

#include <QFile>
#include <QFileInfo>

#include "filesystem.hpp"


namespace Filesystem
{
    namespace
    {
        class LocalFile final: public IFile
        {
        public:
            explicit LocalFile(QString path)
                : m_file(QFileInfo(std::move(path)).absoluteFilePath())
            {

            }

            ~LocalFile() override
            {
                close();
            }

            QByteArrayView asQArrayView() const override
            {
                openView();

                return QByteArrayView(
                    reinterpret_cast<const char*>(m_data),
                    static_cast<qsizetype>(m_size)
                );
            }

            std::span<const std::uint8_t> byteView() const override
            {
                openView();

                return std::span<const std::uint8_t>(
                    reinterpret_cast<const std::uint8_t*>(m_data),
                    static_cast<std::size_t>(m_size)
                );
            }

            // QIODevice interface overrides:
            bool open(QIODeviceBase::OpenMode mode) final
            {
                assert(mode == QIODeviceBase::ReadOnly);

                if (mode != QIODeviceBase::ReadOnly || !m_file.open(mode))
                    return false;

                if (QIODevice::open(mode))
                    return true;

                m_file.close();
                return false;
            }

            void close() final
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

            bool isSequential() const final
            {
                return m_file.isSequential();
            }

            qint64 size() const final
            {
                return m_file.size();
            }

            bool seek(qint64 position) final
            {
                const qint64 oldPosition = m_file.pos();
                if (!m_file.seek(position))
                    return false;

                if (QIODevice::seek(position))
                    return true;

                m_file.seek(oldPosition);
                return false;
            }

            qint64 readData(char* data, qint64 maxSize) final
            {
                return m_file.read(data, maxSize);
            }

            qint64 writeData(const char *, qint64) final
            {
                return -1;
            }

        private:
            mutable QFile m_file;
            mutable uchar* m_data = nullptr;
            mutable qint64 m_size = 0;

            void openView() const
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
        };

    }


    Location::Location(const QStringView& path)
        : m_location(path)
    {

    }


    Location::Location(const std::string_view& path)
        : m_location(QString::fromUtf8(path.data(), path.size()))
    {

    }


    QString Location::toQStr() const
    {
        return url();
    }


    std::string Location::toStr() const
    {
        return url().toStdString();
    }


    QString Location::url() const
    {
        return m_location;
    }


    bool Location::isEmpty() const
    {
        return m_location.isEmpty();
    }



    std::unique_ptr<IFile> openFile(const Location& location)
    {
        auto file = std::make_unique<LocalFile>(location.toQStr());
        file->open(QIODeviceBase::ReadOnly);
        return file;
    }
}
