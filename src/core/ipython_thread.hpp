
#ifndef IPYTHON_THREAD_HPP
#define IPYTHON_THREAD_HPP

#include <memory>

struct PyObject;

struct PyObjectDeleter
{
    void operator()(PyObject* obj) const;
};

template<> class std::unique_ptr<PyObject, PyObjectDeleter>;

struct IPythonThread
{

};

#endif
