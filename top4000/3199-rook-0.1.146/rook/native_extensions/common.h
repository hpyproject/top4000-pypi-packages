#include "Python.h"

namespace native_extensions {
  extern PyObject* Module;
}

#ifndef _PyObject_CAST
#define _PyObject_CAST(op) ((PyObject*)(op))
#endif

#ifndef Py_XSETREF
#define Py_XSETREF(op, op2)                     \
    do {                                        \
        PyObject *_py_tmp = _PyObject_CAST(op); \
        (op) = (op2);                           \
        Py_XDECREF(_py_tmp);                    \
    } while (0)
#endif
