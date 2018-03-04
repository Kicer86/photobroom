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

#include <core/icore_factory_accessor.hpp>
#include <core/ipython_thread.hpp>
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

FaceRecognition::FaceRecognition(ICoreFactoryAccessor* coreAccessor):
    m_pythonThread(coreAccessor->getPythonThread())
{

}


FaceRecognition::~FaceRecognition()
{

}


void FaceRecognition::findFaces(const QString& photo, const Callback<const QVector<QRect> &>& callback) const
{
    m_pythonThread->execute([photo, callback]()
    {
        int status = 0;

        PyRun_SimpleString("import sys");
        PyRun_SimpleString("print( sys.path )");

        PyObjPtr pName( PyUnicode_FromString("find_faces") );
        PyObjPtr pModule( PyImport_Import(pName.get()) );
        if (pModule.get() == nullptr)
        {
            py_utils::dumpExc();
            callback({});
            return;
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
            callback({});
            return;
        }

        QVector<QRect> result;

        const std::size_t facesCount = PyList_Size(pResult.get());
        for(std::size_t i = 0; i < facesCount; i++)
        {
            PyObjPtr item( PyList_GetItem(pResult.get(), i) );

            const QRect rect = tupleToRect(item.get());

            if (rect.isValid())
                result.push_back(rect);
        }

        callback(result);
    });
}
