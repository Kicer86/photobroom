/*
 * Interface for face recognition.
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

#include <functional>
#include <memory>

#include <QVector>

#include "face_recognition_export.h"

#include <database/person_data.hpp>

class QString;
class QRect;

struct ICoreFactoryAccessor;
struct IPythonThread;

namespace Database
{
    struct IDatabase;
    struct IBackendOperator;
}

class FACE_RECOGNITION_EXPORT FaceRecognition final
{
    public:
        template <typename... Args>
        using Callback = std::function<void(Args...)>;

        FaceRecognition(ICoreFactoryAccessor *, Database::IDatabase *, const QString& storage);
        FaceRecognition(const FaceRecognition &) = delete;

        ~FaceRecognition();

        FaceRecognition& operator=(const FaceRecognition &) = delete;

        // Locate faces on given photo. Return results in callback
        void findFaces(const QString &, const Callback<const QVector<QRect> &> &) const;

        // Try to recognize person on given photo and face.
        // Second parameter is a face located by findFaces()
        // Name is returned in callback.
        void nameFor(const QString &, const QRect &, const Callback<const QString &> &) const;

        // Store information about people on given photo.
        // First parameter is path to photo.
        // Second is a list of faces and associated names.
        void store(const QString &, const std::vector<std::pair<QRect, QString>> &) const;

    private:
        struct SharedData;
        std::shared_ptr<SharedData> m_data;
        IPythonThread* m_pythonThread;
        Database::IDatabase* m_db;

        void loadData(Database::IBackendOperator *);
};

#endif // FACERECOGNITION_HPP
