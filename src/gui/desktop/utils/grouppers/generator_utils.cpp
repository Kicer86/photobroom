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

#include "generator_utils.hpp"

#include <QEventLoop>

#include "system/system.hpp"


namespace
{
    const QRegExp loadImages_regExp = QRegExp(R"(^Load\/Image\/.*100% complete.*)");
    const QRegExp mogrify_regExp    = QRegExp(R"(^Mogrify\/Image\/.*)");
    const QRegExp dither_regExp     = QRegExp(R"(^Dither\/Image\/.*100% complete.*)");

    const QRegExp cp_regExp   = QRegExp("^(Creating control points between|Optimizing Variables).*");
    const QRegExp run_regExp  = QRegExp("^Run called.*");
    const QRegExp save_regExp = QRegExp("^saving.*");
}

namespace GeneratorUtils
{

    GenericAnalyzer::GenericAnalyzer(ILogger* logger, int tailLenght):
        QObject(),
        m_logger(logger),
        m_tailLenght(tailLenght)
    {
    }


    void GenericAnalyzer::operator()(QIODevice& device)
    {
        while(device.bytesAvailable() > 0 && device.canReadLine())
        {
            const QByteArray line_raw = device.readLine();
            const QString line(line_raw);

            const QString message = "tool output: " + line.trimmed();
            addMessage(message);
            m_logger->debug(message.toStdString());

            processMessage(message);
        }
    }



    const QStringList& GenericAnalyzer::tail() const
    {
        return m_tail;
    }


    void GenericAnalyzer::addMessage(const QString& m)
    {
        m_tail.append(m);

        if (m_tail.size() > m_tailLenght)
            m_tail.pop_front();
    }


    ///////////////////////////////////////////////////////////////////////////


    ConvertOutputAnalyzer::ConvertOutputAnalyzer(ILogger* logger, int photos_count):
        GenericAnalyzer(logger, 10),
        m_photos_count(photos_count)
    {
    }


    void ConvertOutputAnalyzer::processMessage(const QString& line)
    {
        switch (conversion_data.state)
        {
            case Data::LoadingImages:
            {
                if (loadImages_regExp.exactMatch(line))
                {
                    conversion_data.photos_loaded++;

                    emit progress(conversion_data.photos_loaded * 100 / m_photos_count);
                }
                else if (mogrify_regExp.exactMatch(line))
                {
                    conversion_data.state = conversion_data.BuildingImage;

                    emit operation(tr("Assembling final file"));
                }

                break;
            }

            case Data::BuildingImage:
            {
                if (dither_regExp.exactMatch(line))
                {
                    conversion_data.photos_assembled++;

                    emit progress(conversion_data.photos_assembled * 100 / m_photos_count);
                }

                break;
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////


    AISOutputAnalyzer::AISOutputAnalyzer(ILogger* logger, int photos_count):
        GenericAnalyzer(logger, 10),
        m_photos_count(photos_count)
    {
        stabilization_data.stabilization_steps =  photos_count - 1 // there will be n-1 control points groups
                                                  + 4;             // and 4 optimization steps
    }


    void AISOutputAnalyzer::processMessage(const QString& line)
    {
        switch (stabilization_data.state)
        {
            case Data::StabilizingImages:
                if (cp_regExp.exactMatch(line))
                {
                    stabilization_data.stabilization_step++;

                    emit progress( stabilization_data.stabilization_step * 100 /
                                    stabilization_data.stabilization_steps);
                }
                else if (run_regExp.exactMatch(line))
                {
                    stabilization_data.state = stabilization_data.SavingImages;

                    emit operation(tr("Saving stabilized images"));
                }

                break;

            case Data::SavingImages:
                if (save_regExp.exactMatch(line))
                {
                    stabilization_data.photos_saved++;

                    emit progress( stabilization_data.photos_saved * 100 / m_photos_count );
                }

                break;
        }
    }


    ///////////////////////////////////////////////////////////////////////////


    ProcessRunner::ProcessRunner(): m_work(true)
    {
    }


    void ProcessRunner::operator()(QProcess& pr)
    {
        QEventLoop loop;

        connect(this, &ProcessRunner::stop, &pr, &QProcess::terminate);
        connect(&pr, qOverload<int>(&QProcess::finished), &loop, &QEventLoop::exit);
        connect(&pr, qOverload<int>(&QProcess::finished), this, &ProcessRunner::exitCode);

        if (m_work)
        {
            pr.start();
            loop.exec();
        }

        if (m_work == false)
            throw false;
    }


    void GeneratorUtils::ProcessRunner::cancel()
    {
        m_work = false;

        emit stop();
    }


    int ProcessRunner::getExitCode() const
    {
        return m_exitCode;
    }


    void ProcessRunner::exitCode(int e)
    {
        m_exitCode = e;
    }


    ///////////////////////////////////////////////////////////////////////////


    BreakableTask::BreakableTask(const QString& storage):
        QObject(),
        m_tmpDir(System::getTmpDir("BT_tmp")),
        m_storage(storage),
        m_runner()
    {
        connect(this, &BreakableTask::canceled,
                &m_runner, &GeneratorUtils::ProcessRunner::cancel);
    }


    BreakableTask::~BreakableTask()
    {
    }



    void BreakableTask::perform()
    {
        try
        {
            run();
        }
        catch(bool)
        {
        }
    }


    void BreakableTask::cancel()
    {
        emit canceled();
    }


    QStringList BreakableTask::rotatePhotos(const QStringList& photos,
                                                            const QString& covert,
                                                            ILogger* logger,
                                                            const QString& storage)
    {
        emit operation(tr("Preparing photos"));
        emit progress(0);

        int photo_index = 0;
        QStringList rotated_photos;

        const int p_s = photos.size();
        for (int i = 0; i < p_s; i++)
        {
            const QString& photo = photos[i];
            const QString location = QString("%1/%2.tiff")
                                    .arg(storage)
                                    .arg(photo_index);

            execute(
                logger,
                covert,
                [](QIODevice &) {},
                m_runner,
                "-monitor",                                      // be verbose
                photo,
                "-auto-orient",
                location);

            rotated_photos << location;
            photo_index++;

            emit progress( (i + 1) * 100 /p_s );
        }

        return rotated_photos;
    }

}
