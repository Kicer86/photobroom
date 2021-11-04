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

#include "generator_utils.hpp"

#include <QEventLoop>
#include <QRegularExpression>

#include <core/iexif_reader.hpp>
#include <core/image_tools.hpp>
#include <core/utils.hpp>
#include <system/system.hpp>


namespace
{
    const QRegularExpression loadImages_regExp(R"(^Load\/Image\/.*100% complete.*)");
    const QRegularExpression mogrify_regExp(R"(^Mogrify\/Image\/.*)");
    const QRegularExpression dither_regExp(R"(^Dither\/Image\/.*100% complete.*)");

    const QRegularExpression cp_regExp("^(?:Creating control points between|Optimizing Variables).*");
    const QRegularExpression run_regExp("^Run called.*");
    const QRegularExpression remapping_regExp("^remapping.*");
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
            const QString lineTrimmed = line.trimmed();
            const QString message = "tool output: " + lineTrimmed;

            addMessage(lineTrimmed);
            m_logger->debug(message);

            processMessage(lineTrimmed);
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


    MagickOutputAnalyzer::MagickOutputAnalyzer(ILogger* logger, int photos_count):
        GenericAnalyzer(logger, 10),
        m_photos_count(photos_count)
    {
    }


    void MagickOutputAnalyzer::processMessage(const QString& line)
    {
        switch (conversion_data.state)
        {
            case Data::LoadingImages:
            {
                if (loadImages_regExp.match(line).hasMatch())
                {
                    conversion_data.photos_loaded++;

                    emit progress(conversion_data.photos_loaded * 100 / m_photos_count);
                }
                else if (mogrify_regExp.match(line).hasMatch())
                {
                    conversion_data.state = conversion_data.BuildingImage;

                    emit operation(tr("Assembling final file"));
                }

                break;
            }

            case Data::BuildingImage:
            {
                if (dither_regExp.match(line).hasMatch())
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
                if (cp_regExp.match(line).hasMatch())
                {
                    stabilization_data.stabilization_step++;

                    emit progress( stabilization_data.stabilization_step * 100 /
                                   stabilization_data.stabilization_steps);
                }
                else if (run_regExp.match(line).hasMatch())
                {
                    stabilization_data.state = stabilization_data.SavingImages;

                    emit operation(tr("Saving stabilized images"));
                }

                break;

            case Data::SavingImages:
                if (remapping_regExp.match(line).hasMatch())
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
        connect(&pr, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), &loop, &QEventLoop::exit);
        connect(&pr, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &ProcessRunner::exitCode, Qt::DirectConnection);

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


    BreakableTask::BreakableTask(const QString& storage, IExifReaderFactory& exif):
        QObject(),
        m_tmpDir(System::createTmpDir("BT_tmp", System::Confidential)),
        m_storage(storage),
        m_runner(),
        m_exif(exif)
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


    QStringList BreakableTask::preparePhotos(const QStringList& photos, int scale, const QString& storage)
    {
        emit operation(tr("Preparing photos"));
        emit progress(0);

        IExifReader& exif = m_exif.get();

        int photo_index = 0;
        QStringList prepared_photos;

        const int p_s = photos.size();

        for (int i = 0; i < p_s; i++)
        {
            const QString& photo = photos[i];
            const QString location = QString("%1/%2.tiff")
                                    .arg(storage)
                                    .arg(photo_index);

            const OrientedImage normalized = Image::normalized(photo, exif);
            const QImage scaled = scale == 100?
                normalized.get():
                normalized->scaled(normalized->width() * scale / 100, normalized->height() * scale / 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            scaled.save(location);

            prepared_photos << location;
            photo_index++;

            emit progress( (i + 1) * 100 /p_s );
        }

        return prepared_photos;
    }

}
