
#include "python_utils.hpp"

template class std::unique_ptr<PyObject, py_utils::PyObjectDeleter>;

namespace py_utils
{
    void PyObjectDeleter::operator()(PyObject* obj) const
    {
        Py_DECREF(obj);
    }
}

