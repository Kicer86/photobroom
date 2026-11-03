#ifndef UNIT_TESTS_UTILS_TEMPORARY_FILES_HPP
#define UNIT_TESTS_UTILS_TEMPORARY_FILES_HPP

#include <stdexcept>

#include <QByteArrayView>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringView>
#include <QTemporaryDir>

#include "unit_tests_utils/location.hpp"


namespace UnitTests
{
    /**
     * Owns a temporary directory and creates Filesystem::Locations inside it.
     *
     * This keeps test-only Location construction in one place and guarantees
     * that files created by a test are removed together with this object.
     */
    class TemporaryFiles
    {
    public:
        TemporaryFiles()
        {
            if (!m_directory.isValid())
                throw std::runtime_error("Could not create a temporary directory");
        }

        [[nodiscard]] Filesystem::Location createFile(const QStringView& relativePath,
                                                      const QByteArrayView& contents = {}) const
        {
            const QString path = absolutePath(relativePath);
            const QFileInfo fileInfo(path);

            if (!QDir().mkpath(fileInfo.absolutePath()))
                throw std::runtime_error("Could not create a temporary file directory");

            QFile file(path);
            if (!file.open(QIODevice::WriteOnly)
                || file.write(contents.data(), contents.size()) != contents.size())
            {
                throw std::runtime_error("Could not write a temporary file");
            }

            return UnitTests::makeLocation(path);
        }

        [[nodiscard]] Filesystem::Location location(const QStringView& relativePath) const
        {
            return UnitTests::makeLocation(absolutePath(relativePath));
        }

    private:
        QString absolutePath(const QStringView& relativePath) const
        {
            const QString path = QDir::cleanPath(relativePath.toString());
            if (path.isEmpty() || path == QStringLiteral(".") || path == QStringLiteral("..")
                || path.startsWith(QStringLiteral("../")) || QDir::isAbsolutePath(path))
            {
                throw std::invalid_argument("Temporary file path must be relative");
            }

            return m_directory.filePath(path);
        }

        QTemporaryDir m_directory;
    };
}

#endif
