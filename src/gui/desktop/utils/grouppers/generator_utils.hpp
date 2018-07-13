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

    template<typename T, typename L, typename ...Args>
    void execute(ILogger* logger,
                 const QString& executable,
                 T&& outputDataCallback,
                 L&& launcher,
                 const Args... args)
    {
        QStringList arguments;
        append(arguments, args...);

        QProcess pr;
        pr.setProcessChannelMode(QProcess::MergedChannels);

        QObject::connect(&pr, &QIODevice::readyRead, [&outputDataCallback, &pr]()
        {
            outputDataCallback(pr);
        });

        pr.setProgram(executable);
        pr.setArguments(arguments);

        const std::string info_message =
            QString("Executing %1 %2").arg(executable).arg(arguments.join(" ")).toStdString();

        logger->info(info_message);

        launcher(pr);
    }


    class ConvertOutputAnalyzer: public QObject
    {
            Q_OBJECT

        public:
            ConvertOutputAnalyzer(ILogger* logger, int photos_count);

            void operator()(QIODevice& device);

        signals:
            void operation(const QString &);
            void progress(int);
            void finished(const QString &);

        private:
            struct
            {
                int photos_loaded = 0;
                int photos_assembled = 0;

                enum
                {
                    LoadingImages,
                    BuildingImage,
                } state = LoadingImages;

            } conversion_data;

            const int m_photos_count;
            ILogger* m_logger;
    };


    class AISOutputAnalyzer: public QObject
    {
            Q_OBJECT

        public:
            AISOutputAnalyzer(ILogger* logger, int photos_count);

            void operator()(QIODevice& device);

        signals:
            void operation(const QString &);
            void progress(int);
            void finished(const QString &);

        private:
            struct
            {
                int stabilization_steps = 0;
                int stabilization_step = 0;
                int photos_saved = 0;

                enum
                {
                    StabilizingImages,
                    SavingImages,
                } state = StabilizingImages;
            } stabilization_data;

            const int m_photos_count;
            ILogger* m_logger;
    };


    class ProcessRunner: public QObject
    {
            Q_OBJECT

        public:
            ProcessRunner();

            void operator()(QProcess &);
            void cancel();

        private:

        signals:
            void stop();
    };
}

#endif // GENERATORUTILS_HPP
