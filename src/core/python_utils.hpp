
#ifndef PYTHON_UTILS_HPP
#define PYTHON_UTILS_HPP

#include <memory>
#include "Python.h"

#include "core_export.h"

namespace py_utils
{
    struct PyObjectDeleter
    {
        void operator()(PyObject* obj) const;
    };
}

extern template class CORE_EXPORT std::unique_ptr<PyObject, py_utils::PyObjectDeleter>;
typedef std::unique_ptr<PyObject, py_utils::PyObjectDeleter> PyObjPtr;


#endif
