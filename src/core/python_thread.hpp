/*
 * Utils responsible for managing Python's thread.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#ifndef PYTHON_THREAD_HPP
#define PYTHON_THREAD_HPP

#include <thread>

#include <OpenLibrary/putils/ts_queue.hpp>

#include "ipython_thread.hpp"

#include "core_export.h"

class CORE_EXPORT PythonThread final: public IPythonThread
{
    public:
        PythonThread();
        ~PythonThread();

        void execute(const std::function<void ()> &) override;

    private:
        struct Impl;

        std::unique_ptr<Impl> m_impl;
        std::thread m_pythonThread;

        void thread();
};

#endif // PYTHON_THREAD_HPP
