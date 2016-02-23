
#ifndef GDBWRAPPER_HPP
#define GDBWRAPPER_HPP

#include <QString>

#include "../idebugger.hpp"


class GDBWrapper: public IDebugger
{
    public:
        GDBWrapper(const QString &);

        virtual bool attach(qint64 pid, qint64 tid) override;
        virtual std::vector<std::string> getBackTrace() override;

    private:
        const QString m_gdb_path;
};

#endif // GDBWRAPPER_HPP
