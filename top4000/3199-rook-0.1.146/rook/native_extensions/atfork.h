// Ensure that Python.h is included before any other header.
#include "common.h"


namespace native_extensions {

extern int python_fork_handler_called;

PyObject* RegisterPreforkCallback(PyObject* self, PyObject* py_args);

}  // namespace native_extensions