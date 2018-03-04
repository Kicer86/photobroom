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
#include <vector>

class QString;
class QRect;

struct ICoreFactoryAccessor;
struct IPythonThread;

class FaceRecognition final
{
    public:
        template <typename... Args>
        using Callback = std::function<void(Args...)>;

        FaceRecognition(ICoreFactoryAccessor *);
        FaceRecognition(const FaceRecognition &) = delete;

        ~FaceRecognition();

        FaceRecognition& operator=(const FaceRecognition &) = delete;

        void findFaces(const QString &, const Callback<const std::vector<QRect> &> &) const;

    private:
        IPythonThread* m_pythonThread;
};

#endif // FACERECOGNITION_HPP
