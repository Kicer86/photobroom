/*
 * Photo Broom - photos management tool.
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
#include <core/itask_executor.hpp>

struct IExifReaderFactory;
struct ITmpDir;

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

        const QString info_message = QString("Executing %1 %2").arg(executable).arg(arguments.join(" "));

        logger->info(info_message);

        launcher(pr);
    }


    class GenericAnalyzer: public QObject
    {
            Q_OBJECT

        public:
            GenericAnalyzer(ILogger *, int tailLenght);
            virtual ~GenericAnalyzer() = default;

            const QStringList& tail() const;

            virtual void operator()(QIODevice& device) final;

        protected:
            virtual void processMessage(const QString &) = 0;

        private:
            QStringList m_tail;
            ILogger* m_logger;
            int m_tailLenght;

            void addMessage(const QString &);

        signals:
            void operation(const QString &);
            void progress(int);
            void finished(const QString &);
    };


    class MagickOutputAnalyzer: public GenericAnalyzer
    {
            Q_OBJECT

        public:
            MagickOutputAnalyzer(ILogger* logger, int photos_count);

            void processMessage(const QString &) override;

        private:
            struct Data
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
    };


    class AISOutputAnalyzer: public GenericAnalyzer
    {
            Q_OBJECT

        public:
            AISOutputAnalyzer(ILogger* logger, int photos_count);

            void processMessage(const QString &) override;

        private:
            struct Data
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
    };


    class ProcessRunner: public QObject
    {
            Q_OBJECT

        public:
            ProcessRunner();

            void operator()(QProcess &);      // throws `bool` if action was cancelled or was launched again after cancelation
            void cancel();

            int getExitCode() const;

        private:
            int m_exitCode;
            bool m_work;

            void exitCode(int);

        signals:
            void stop();
    };


    class BreakableTask: public QObject, public ITaskExecutor::ITask
    {
            Q_OBJECT

        public:
            BreakableTask(const QString& storage, IExifReaderFactory &);
            virtual ~BreakableTask();

            void perform() override final;
            void cancel();

        protected:
            std::shared_ptr<ITmpDir> m_tmpDir;
            const QString m_storage;
            ProcessRunner m_runner;
            IExifReaderFactory& m_exif;

            virtual void run() = 0;

            QStringList preparePhotos(const QStringList& photos, int scale);

        signals:
            void operation(const QString &) const;
            void progress(int) const;
            void finished(const QString &) const;
            void canceled() const;
            void error(const QString &, const QStringList &) const;
    };
}

#endif // GENERATORUTILS_HPP
