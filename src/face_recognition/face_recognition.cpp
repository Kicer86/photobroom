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

#include "face_recognition.hpp"

#include <cassert>
#include <memory>
#include <string>

#include <Python.h>

#include <QString>
#include <QRect>

#include <core/python_utils.hpp>
#include <system/filesystem.hpp>


namespace
{
    QRect tupleToRect(PyObject* tuple)
    {
        QRect result;

        std::vector<long> rect;
        const std::size_t coordinates = PyTuple_Size(tuple);

        if (coordinates == 4)
        {
            for(std::size_t i = 0; i < coordinates; i++)
            {
                PyObject* part = PyTuple_GetItem(tuple, i);

                const long n = PyLong_AsLong(part);
                rect.push_back(n);
            }

            result = QRect(rect[0], rect[1], rect[2], rect[3]);
        }

        return result;
    }
}

FaceRecognition::FaceRecognition()
{
    Py_Initialize();

    updateSearchPath();
}


FaceRecognition::~FaceRecognition()
{
    Py_Finalize();
}


std::vector<QRect> FaceRecognition::findFaces(const QString& photo) const
{
    int status = 0;

    PyObjPtr pName( PyUnicode_FromString("find_faces") );
    PyObjPtr pModule( PyImport_Import(pName.get()) );
    if (pModule.get() == nullptr)
    {
        py_utils::dumpExc();
        return {};
    }

    PyObjPtr pDict( PyModule_GetDict(pModule.get()) );
    PyObjPtr pFunc( PyDict_GetItemString(pDict.get(), "find_faces") );
    PyObjPtr pArgs( PyTuple_New(1) );
    PyObjPtr pPath( PyUnicode_FromString(photo.toStdString().c_str()) );

    status = PyTuple_SetItem(pArgs.get(), 0, pPath.get());

    PyObjPtr pResult( PyObject_CallObject(pFunc.get(), pArgs.get()) );

    // Print a message if calling the method failed.
    if (pResult == nullptr)
    {
        py_utils::dumpExc();
        return {};
    }

    std::vector<QRect> result;

    const std::size_t facesCount = PyList_Size(pResult.get());
    for(std::size_t i = 0; i < facesCount; i++)
    {
        PyObjPtr item( PyList_GetItem(pResult.get(), i) );

        const QRect rect = tupleToRect(item.get());

        if (rect.isValid())
            result.push_back(rect);
    }

    return result;
}


void FaceRecognition::updateSearchPath() const
{
    int status = 0;

    // add path to scripts to Python's search path
    // https://mail.python.org/pipermail/capi-sig/2013-May/000590.html
    PyObject* scriptsPath = PyUnicode_FromString(FileSystem().getScriptsPath().toStdString().c_str());
    PyObject* pPathsList = PySys_GetObject("path");
    status = PyList_Append(pPathsList, scriptsPath);
    assert(status == 0);

    status = PySys_SetObject("path", pPathsList);
    assert(status == 0);
}
