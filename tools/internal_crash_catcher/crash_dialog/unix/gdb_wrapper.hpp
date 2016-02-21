
#ifndef GDBWRAPPER_HPP
#define GDBWRAPPER_HPP


#include "../idebugger.hpp"


class GDBWrapper: public IDebugger
{
    public:
        GDBWrapper();

        virtual bool attach(qint64 pid, qint64 tid);
        virtual std::vector<std::string> getBackTrace();
};

#endif // GDBWRAPPER_HPP
