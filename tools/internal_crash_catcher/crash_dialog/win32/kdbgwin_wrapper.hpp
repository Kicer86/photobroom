/*
 * Win32 debugger
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "idebugger.hpp"


class KDbgWinWrapper: public IDebugger
{
    public:
        KDbgWinWrapper();
        ~KDbgWinWrapper();

        bool init(qint64 pid, qint64 tid, const QString& exec) override;
        void requestBacktrace( const std::function<void(const std::vector<QString> &)> & ) override;

        const QString& exec() const override;

    private:
        qint64 m_pid;
        qint64 m_tid;
        QString m_exec;

        bool enableDebugPrivilege();
};
