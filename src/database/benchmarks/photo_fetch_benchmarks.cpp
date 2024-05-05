
#include <QDir>
#include <QTime>
#include <QTemporaryDir>

#include <nanobench.h>

#include <database/backends/memory_backend/memory_backend.hpp>
#include <database/backends/sql_backends/sqlite_backend/backend.hpp>
#include <database/filter.hpp>
#include <database/project_info.hpp>

#include <unit_tests_utils/empty_logger.hpp>

namespace
{
    void fillBackendWithData(Database::IBackend& backend)
    {
        std::vector<Photo::DataDelta> photos;

        for (int i = 0; i < 1000; i++)
        {
            Photo::DataDelta data;
            data.insert<Photo::Field::Path>(QString("path %1").arg(i % 11));

            Tag::TagsList tags;
            tags.emplace(Tag::Types::Time, QTime(i % 5 + 10, 0, 0));
            tags.emplace(Tag::Types::Event, QString("Event %1").arg(i % 10));
            tags.emplace(Tag::Types::Place, QString("Place %1").arg(i % 9));
            data.insert<Photo::Field::Tags>(tags);

            photos.push_back(data);
        }

        backend.addPhotos(photos);
    }

    std::unique_ptr<Database::IBackend> setupMemoryBackend()
    {
        auto memoryBackend = std::make_unique<Database::MemoryBackend>();

        return memoryBackend;
    }

    std::unique_ptr<Database::IBackend> setupSQLiteBackend(ILogger& logger, QTemporaryDir& wd)
    {
        std::unique_ptr<Database::IBackend> sqliteBackend = std::make_unique<Database::SQLiteBackend>(nullptr, &logger);

        const QString wd_path = wd.path();
        const QString db_path = wd_path;
        QDir().mkdir(db_path);
        Database::ProjectInfo prjInfo(db_path + "/db", "benchmark");

        sqliteBackend->init(prjInfo);

        return sqliteBackend;
    }


    void for_each_backend(auto init, auto op, std::string_view name)
    {
        {
            auto memoryBackend = setupMemoryBackend();
            init(*memoryBackend);

            ankerl::nanobench::Bench().run("Memory backend: " + std::string(name), [&]
            {
                op(*memoryBackend);
            });
        }

        {
            QTemporaryDir tmpDir;
            EmptyLogger logger;
            std::unique_ptr<Database::IBackend> sqliteBackend = setupSQLiteBackend(logger, tmpDir);

            fillBackendWithData(*sqliteBackend);
            ankerl::nanobench::Bench().run("SQLite backend: " + std::string(name), [&]
            {
                op(*sqliteBackend);
            });

            sqliteBackend->closeConnections();
        }
    }
}


void BM_get_photo_delta()
{
    for_each_backend(fillBackendWithData, [](Database::IBackend& backend)
    {
        const auto ids = backend.photoOperator().getPhotos(Database::FilterPhotosWithPath("path 1"));

        std::vector<Photo::DataDelta> photos;
        for(const auto id: ids)
            backend.getPhotoDelta(id, {});

        ankerl::nanobench::doNotOptimizeAway(photos);
    },
    "getPhotoDelta()"
    );
}
