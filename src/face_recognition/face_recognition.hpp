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

#ifndef FACERECOGNITION_HPP
#define FACERECOGNITION_HPP

#include <QVector>
#include <memory>

#include <database/person_data.hpp>
#include "face_recognition_export.h"

class QString;
class QRect;

struct ICoreFactoryAccessor;
struct IExifReader;
struct ILogger;
struct ITmpDir;
struct FacesData;

namespace Database
{
    struct IDatabase;
}

class FACE_RECOGNITION_EXPORT FaceRecognition final
{
    public:
        FaceRecognition(ICoreFactoryAccessor *);
        FaceRecognition(const FaceRecognition &) = delete;

        ~FaceRecognition();

        FaceRecognition& operator=(const FaceRecognition &) = delete;

        // Locate faces on given photo.
        [[deprecated]] QVector<QRect> fetchFaces(const QString &) const;

        // Try to recognize person on given photo and face.
        // Second parameter is a face located by fetchFaces()
        // Third parameter is a path to directory with known faces
        [[deprecated]] Person::Id recognize(const QString &, const QRect &, Database::IDatabase *) const;

        // Optimize representative photo
        // from given set of faces, choose one with best parameters
        [[deprecated]] QString best(const QStringList& faces);

    private:
        struct Data;
        std::unique_ptr<Data> m_data;
};

#endif // FACERECOGNITION_HPP
