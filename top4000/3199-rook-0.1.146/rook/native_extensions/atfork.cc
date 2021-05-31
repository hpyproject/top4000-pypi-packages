#include "common.h"
#include "atfork.h"

#include <pthread.h>
#include <stdio.h>

namespace native_extensions {
    static void preforkCallback() {
        PyObject* python_fork_handler_called = PyObject_GetAttrString(Module, "python_fork_handler_called");

        if (python_fork_handler_called != NULL) {
            if (PyLong_AsLong(python_fork_handler_called) != 1) {
                printf("[Rookout] Rookout does not support running in forking processes. Please start rook only after forking\n"
                       " For more information, see https://docs.rookout.com/docs/sdk-setup.html#pre-forking-servers\n");
            }

            Py_DECREF(python_fork_handler_called);
        }
    }

    PyObject* RegisterPreforkCallback(PyObject* self, PyObject* py_args) {
        if (pthread_atfork(preforkCallback, NULL, NULL)) {
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    }
}
