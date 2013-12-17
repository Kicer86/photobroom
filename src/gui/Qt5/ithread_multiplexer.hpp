
#ifndef GUI_ITHREAD_MULTIPLEXER_HPP
#define GUI_ITHREAD_MULTIPLEXER_HPP

template<typename T>
struct IThreadMultiplexer
{
    struct IGetter
    {
        virtual ~IGetter() {}
        virtual void getSignal(const T &) = 0;  //main thread will read here
    };

    virtual ~IThreadMultiplexer() {}

    virtual void send(const T &) = 0;     //threads should write here
    virtual void setGetter(IGetter *) = 0;

};

#endif
