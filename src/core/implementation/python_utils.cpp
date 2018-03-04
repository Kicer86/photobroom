
#include "python_utils.hpp"

template class std::unique_ptr<PyObject, py_utils::PyObjectDeleter>;

namespace py_utils
{
    void PyObjectDeleter::operator()(PyObject* obj) const
    {
        Py_DECREF(obj);
    }


    std::string ObjectToString(PyObject* obj)
    {
        PyObjPtr str( PyObject_Str(obj) );
        const char* c_str = PyUnicode_AsUTF8(str.get());

        return c_str;
    }

    // Based on:
    // https://stackoverflow.com/a/15907460/1749713
    // https://stackoverflow.com/a/43372878/1749713
    std::string dumpExc()
    {
        PyObject *ptype, *pvalue, *ptraceback;

        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

        PyObjPtr type(ptype), value(pvalue), traceback(ptraceback);

        const std::string value_str = ObjectToString(pvalue);

        /* See if we can get a full traceback */
        PyObjPtr module_name( PyUnicode_FromString("traceback") );
        PyObjPtr pyth_module( PyImport_Import(module_name.get()) );

        if (pyth_module.get())
        {
            PyObjPtr pyth_func( PyObject_GetAttrString(pyth_module.get(), "format_exception") );
            if (pyth_func && PyCallable_Check(pyth_func.get()))
            {
                PyObjPtr pyth_val( PyObject_CallFunctionObjArgs(pyth_func.get(), type.get(), value.get(), traceback.get(), NULL) );

                std::string traceback_str = ObjectToString(pyth_val.get());
            }
        }
    }
}

