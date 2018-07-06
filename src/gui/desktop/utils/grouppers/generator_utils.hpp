/*
 * generic code for generators
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

#ifndef GENERATORUTILS_HPP
#define GENERATORUTILS_HPP

#include <functional>

#include <QProcess>
#include <QStringList>

#include <core/ilogger.hpp>

namespace GeneratorUtils
{
    template<typename T>
    void append(QStringList& list, const T& arg)
    {
        list << arg;
    }

    template<typename T, typename ...Args>
    void append(QStringList& list, const T& arg, Args... args)
    {
        list << arg;
        append(list, args...);
    }

    template<typename ...Args>
    void execute(ILogger* logger,
                 const QString& executable,
                 const std::function<void(QIODevice &)>& outputDataCallback,
                 const std::function<void(QProcess &)>& launcher,
                 const Args... args)
    {
        QStringList arguments;
        append(arguments, args...);

        QProcess pr;
        pr.setProcessChannelMode(QProcess::MergedChannels);

        if (outputDataCallback)
            QObject::connect(&pr, &QIODevice::readyRead, std::bind(outputDataCallback, std::ref(pr)));

        pr.setProgram(executable);
        pr.setArguments(arguments);

        const std::string info_message =
            QString("Executing %1 %2").arg(executable).arg(arguments.join(" ")).toStdString();

        logger->info(info_message);

        launcher(pr);
    }
}

#endif // GENERATORUTILS_HPP
