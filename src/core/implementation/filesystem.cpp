
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
                if (m_data != nullptr)
                    m_file.unmap(m_data);
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

            qint64 readData(char* data, qint64 maxSize) final
            {
                return m_file.read(data, maxSize);
            }

            [[deprecated("Use byteView() or asQArrayView() instead")]]
            std::string path() const override
            {
                return m_file.fileName().toStdString();
            }

        private:
            mutable QFile m_file;
            mutable uchar* m_data = nullptr;
            mutable qint64 m_size = 0;

            void openView() const
            {
                if (not m_data)
                {
                    if (m_file.open(QIODevice::ReadOnly) == false)
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


        class Device final: public QIODevice
        {
        public:
            Device(const Location& location)
                : m_file(openFile(location))
            {
            }

            qint64 readData(char* data, qint64 maxSize) final
            {
                return m_file->readData(data, maxSize);
            }

            qint64 writeData(const char *data, qint64 maxSize) final
            {
                return 0;
            }

        private:
            std::unique_ptr<IFile> m_file;
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


    std::unique_ptr<QIODevice> openAsDevice(const Location& location)
    {
        return std::make_unique<Device>(location);
    }


    std::unique_ptr<IFile> openFile(const Location& location)
    {
        const auto path = location.toQStr();

        if (QFile::exists(path))
        {
            auto file = std::make_unique<LocalFile>(path);
            return file;
        }
        else
            return nullptr;
    }
}
