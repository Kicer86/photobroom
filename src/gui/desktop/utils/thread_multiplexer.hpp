
// The purpose of this class is to pass signals from many threads into specific one


#ifndef GUI_THREAD_MULTIPLEXER_HPP
#define GUI_THREAD_MULTIPLEXER_HPP

#include <QObject>

#include "ithread_multiplexer.hpp"

class PhotoInfoUpdater;

class ThreadMultiplexer: public QObject, public IThreadMultiplexer<PhotoInfoUpdater *>
{
        Q_OBJECT

    public:
        ThreadMultiplexer();
        ThreadMultiplexer(const ThreadMultiplexer &) = delete;
        virtual ~ThreadMultiplexer();

        ThreadMultiplexer& operator=(const ThreadMultiplexer &) = delete;

        virtual void send(PhotoInfoUpdater* const& );
        virtual void setGetter(IGetter* );

    private:
        IGetter* m_getter;

    private slots:
        void propagated(PhotoInfoUpdater *);

    signals:
        void propagate(PhotoInfoUpdater *);
};

#endif
