/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef IDATABASE_HPP
#define IDATABASE_HPP

#include <vector>
#include <functional>
#include <memory>
#include <set>
#include <source_location>
#include <string>

#include <QObject>

#include <core/tag.hpp>

#include "database_status.hpp"
#include "filter.hpp"
#include "group.hpp"
#include "ibackend.hpp"
#include "person_data.hpp"

#include "database_export.h"

struct ILogger;
struct IConfiguration;


namespace Database
{
    template<typename T>
    concept BackendTask = std::invocable<T, IBackend *>;

    struct IPhotoInfoCache;
    struct IPhotoInfoCreator;
    struct IDatabaseClient;
    struct ProjectInfo;


    // class responsible for running tasks meant to be executed in db's thread on IBackend
    struct DATABASE_EXPORT IDatabaseThread
    {
        virtual ~IDatabaseThread() = default;

        template<typename Callable> requires std::is_invocable_v<Callable, IBackend &>
        void exec(Callable&& f, const std::string& name = std::source_location::current().function_name())
        {
            auto task = std::make_unique<Task<Callable>>(std::forward<Callable>(f), name);
            execute(std::move(task));
        }

        struct ITask
        {
            virtual ~ITask() = default;
            virtual void run(IBackend &) = 0;
            virtual std::string name() = 0;
        };

        protected:
            template<typename Callable>
            struct Task final: ITask
            {
                Task(Callable&& f, const std::string& name)
                    : m_f(std::forward<Callable>(f))
                    , m_name(name)
                {

                }

                void run(IBackend& backend) override
                {
                    m_f(backend);
                }

                std::string name() override
                {
                    return m_name;
                }

                typedef typename std::remove_reference<Callable>::type Callable_T;  // be sure we store copy of object, not reference or something
                Callable_T m_f;
                std::string m_name;
            };

            virtual void execute(std::unique_ptr<ITask> &&) = 0;
    };

    //Database interface.
    //A bridge between clients and backend.
    struct DATABASE_EXPORT IDatabase: IDatabaseThread
    {
        template <typename... Args>
        using Callback = std::function<void(Args...)>;

        virtual ~IDatabase() = default;

        // other
        virtual IBackend& backend() = 0;

        //init backend - connect to database or create new one
        virtual void init(const ProjectInfo &, const Callback<const BackendStatus &> &) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };
}

#endif
