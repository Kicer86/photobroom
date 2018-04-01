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

#include <QVector>

#include "face_recognition_export.h"

class QString;
class QRect;

struct ICoreFactoryAccessor;
struct IPythonThread;

namespace Database
{
    struct IDatabase;
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

        void findFaces(const QString &, const Callback<const QVector<QRect> &> &) const;
        void nameFor(const QString &, const QRect &, const Callback<const QString &> &) const;

        void store(const QString &, const std::vector<std::pair<QRect, QString>> &) const;

    private:
        const QString m_storage;
        IPythonThread* m_pythonThread;
        Database::IDatabase* m_db;
};

#endif // FACERECOGNITION_HPP
