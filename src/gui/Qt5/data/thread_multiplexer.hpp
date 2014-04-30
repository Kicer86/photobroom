
// The purpose of this class is to pass signals from many threads into specific one


#ifndef GUI_THREAD_MULTIPLEXER_HPP
#define GUI_THREAD_MULTIPLEXER_HPP

#include <QObject>

#include "ithread_multiplexer.hpp"

class PhotoInfoGenerator;

class ThreadMultiplexer: public QObject, public IThreadMultiplexer<PhotoInfoGenerator *>
{
        Q_OBJECT

    public:
        ThreadMultiplexer();
        ThreadMultiplexer(const ThreadMultiplexer &) = delete;
        virtual ~ThreadMultiplexer();

        ThreadMultiplexer& operator=(const ThreadMultiplexer &) = delete;

        virtual void send(PhotoInfoGenerator* const& );
        virtual void setGetter(IGetter* );

    private:
        IGetter* m_getter;

    private slots:
        void propagated(PhotoInfoGenerator *);

    signals:
        void propagate(PhotoInfoGenerator *);
};

#endif
