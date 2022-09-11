
#include "photo_crawler.hpp"

#include <cassert>
#include <thread>

#include <QString>

#include "ifile_system_scanner.hpp"
#include "ianalyzer.hpp"

namespace
{

    struct FileNotifier: IFileNotifier
    {
        FileNotifier(IAnalyzer* analyzer, IMediaNotification* notifications): m_analyzer(analyzer), m_notifications(notifications) {}
        FileNotifier(const FileNotifier &) = delete;

        FileNotifier& operator=(const FileNotifier &) = delete;

        virtual void found(const QString& file) override
        {
            if (m_analyzer->isMediaFile(file))
                m_notifications->found(file);
        }

        virtual void finished() override
        {
            m_notifications->finished();
        }

        IAnalyzer* m_analyzer;
        IMediaNotification* m_notifications;
    };

}


struct PhotoCrawler::Impl
{
    Impl(std::unique_ptr<IFileSystemScanner>&& scanner, std::unique_ptr<IAnalyzer>&& analyzer):
        m_scanner( std::move(scanner) ),
        m_analyzer( std::move(analyzer) ),
        m_thread()
    {

    }

    ~Impl()
    {
        releaseThread();
    }

    Impl(const Impl &) = delete;
    Impl& operator=(const Impl &) = delete;

    std::unique_ptr<IFileSystemScanner> m_scanner;
    std::unique_ptr<IAnalyzer> m_analyzer;
    std::thread m_thread;

    void run(const QStringList& paths, IMediaNotification* notifications)
    {
        releaseThread();

        m_thread = std::thread(&Impl::thread, this, paths, notifications );
    }

    void releaseThread()
    {
        m_scanner->stop();

        if (m_thread.joinable())
            m_thread.join();
    }

    //thread function
    void thread(const QStringList& paths, IMediaNotification* notifications)
    {
        FileNotifier notifier(m_analyzer.get(), notifications);

        m_scanner->getFilesFor(paths, &notifier);
    }
};


PhotoCrawler::PhotoCrawler(std::unique_ptr<IFileSystemScanner>&& scanner,
                           std::unique_ptr<IAnalyzer>&& analyzer): m_impl(new Impl( std::move(scanner), std::move(analyzer) ))
{

}


PhotoCrawler::~PhotoCrawler()
{

}


void PhotoCrawler::crawl(const QStringList& paths, IMediaNotification* notifications)
{
    m_impl->run(paths, notifications);
}


void PhotoCrawler::stop()
{
    m_impl->releaseThread();
}
