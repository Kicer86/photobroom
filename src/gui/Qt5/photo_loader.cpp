
#include "photo_loader.hpp"

#include <thread>

#include <OpenLibrary/palgorithm/ts_queue.hpp>

static void trampoline(PhotoLoader::Data *);


struct PhotoLoader::Data
{
    struct Task
    {
        QString path;
        IPhotoLoader::INotifier *notifier;
    };


    //TODO: configurable?
    Data(): m_tasks(2048), m_taskEater(trampoline, this)
    {

    }

    void addTask(const QString& path, IPhotoLoader::INotifier* notifier)
    {
    }

    private:
        friend void trampoline(PhotoLoader::Data *data);
        TS_Queue<Task> m_tasks;
        std::thread m_taskEater;

        void taskEater()
        {
            while(true)
            {
            }
        }

};

static void trampoline(PhotoLoader::Data *data)
{
    data->taskEater();
}

PhotoLoader::PhotoLoader()
{

}


PhotoLoader::~PhotoLoader()
{

}

void PhotoLoader::generateThumbnail(const QString& path, IPhotoLoader::INotifier *notifier)
{

}


QPixmap PhotoLoader::getThumbnailFor(const QString& path)
{

}
