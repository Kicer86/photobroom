
#include "photo_loader.hpp"

#include <thread>
#include <unordered_map>
#include <memory>

#include <omp.h>

#include <QPixmap>

#include <OpenLibrary/palgorithm/ts_queue.hpp>
#include <OpenLibrary/palgorithm/ts_resource.hpp>

static void trampoline(PhotoLoader::Data *);

namespace
{
    //TODO: remove, use config
    const int photoWidth = 120;

    struct hash
    {
        std::size_t operator()(const QString& str) const
        {
            std::hash<std::string> hash_fn;
            const std::size_t h = hash_fn(str.toStdString());
            return h;
        }
    };
}


struct PhotoLoader::Data
{
    struct Task
    {
        Task(const QString& p, IPhotoLoader::INotifier* n): path(p), notifier(n) {}
        Task(const Task &) = default;
        Task(): path(), notifier(nullptr) {}

        Task& operator=(const Task &) = default;

        QString path;
        IPhotoLoader::INotifier *notifier;
    };

    //TODO: configurable?
    Data(): m_tasks(2048), m_results(), m_taskEater(trampoline, this)
    {

    }

    ~Data()
    {
        m_tasks.break_popping();
        assert(m_taskEater.joinable());
        m_taskEater.join();
    }

    void addTask(const QString& path, IPhotoLoader::INotifier* notifier)
    {
        Task task(path, notifier);

        m_tasks.push_back(task);
    }

    QPixmap getPixmap(const QString& path)
    {
        auto results = m_results.get();

        auto f = results->find(path);
        QPixmap result = f != results->end()? f->second: QPixmap();

        if (f != results->end())
            results->erase(f);

        return result;
    }

    private:
        friend void trampoline(PhotoLoader::Data *data);
        TS_Queue<Task> m_tasks;
        ThreadSafeResource<std::unordered_map<QString, QPixmap, hash>> m_results;
        std::thread m_taskEater;

        void taskEater()
        {
            #pragma omp parallel
            {
                const int id = omp_get_thread_num();
                **(ThreadSafeOutput.get()) << "Starting QPixmap resizer's thread #" << id << std::endl;

                while(true)
                {
                    boost::optional<Task> task = m_tasks.pop_front();

                    if (task)
                        eatTask(task.get());
                    else
                        break;
                }

                **(ThreadSafeOutput.get()) << "Quitting QPixmap resizer's thread #" << id << std::endl;
            }
        }

        void eatTask(const Task& task)
        {
            QPixmap pixmap(task.path);
            QPixmap thumbnail = pixmap.scaled(photoWidth, photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            auto data = std::make_pair(task.path, thumbnail);

            m_results.get()->insert(data);

            task.notifier->thumbnailReady(task.path);
        }
};


static void trampoline(PhotoLoader::Data* data)
{
    data->taskEater();
}


PhotoLoader::PhotoLoader():m_data(new Data)
{

}


PhotoLoader::~PhotoLoader()
{

}

void PhotoLoader::generateThumbnail(const QString& path, IPhotoLoader::INotifier *notifier)
{
    m_data->addTask(path, notifier);
}


QPixmap PhotoLoader::getThumbnailFor(const QString& path)
{
    return m_data->getPixmap(path);
}
