
#ifndef FILESYSTEM_LOCALFILE_HPP_INCLUDED
#define FILESYSTEM_LOCALFILE_HPP_INCLUDED

#include <cstdint>
#include <span>

#include <QFile>

#include "filesystem.hpp"


namespace Filesystem
{
    class LocalFile final: public IFile
    {
    public:
        explicit LocalFile(QString path);
        ~LocalFile() override;

        QByteArrayView asQArrayView() const override;
        std::span<const std::uint8_t> byteView() const override;

        bool open(QIODeviceBase::OpenMode mode) final;
        void close() final;
        bool isSequential() const final;
        qint64 size() const final;
        bool seek(qint64 position) final;

    private:
        mutable QFile m_file;
        mutable uchar* m_data = nullptr;
        mutable qint64 m_size = 0;

        qint64 readData(char* data, qint64 maxSize) final;
        qint64 writeData(const char* data, qint64 maxSize) final;
        void openView() const;
    };
}

#endif
