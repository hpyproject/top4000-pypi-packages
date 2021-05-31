#include "common.h"
#include "frameobject.h"

#ifndef PYPY_VERSION
namespace native_extensions {

static PyObject* import_hook = NULL;
static bool strip_tracebackhide_frames = true;

/* Remove frames from the traceback when they have __tracebackhide__ in the frame locals (as in pypy),
 * except in Verbose mode. Loosely based on import.c:remove_importlib_frames from CPython 3.3+ */
    void remove_tracebackhide_frames() {

        PyObject *exception, *value, *base_tb, *tb = NULL;
        PyObject **prev_link, **last_not_hidden_link = NULL;



        PyErr_Fetch(&exception, &value, &base_tb);
        if ((exception == NULL) || Py_VerboseFlag) {
            goto done;
        }

        prev_link = &base_tb;
        last_not_hidden_link = &base_tb;
        tb = base_tb;
        while (tb != NULL) {
            PyTracebackObject *traceback = (PyTracebackObject *)tb;
            PyObject *next = (PyObject *) traceback->tb_next;
            PyFrameObject *frame = traceback->tb_frame;


            assert(PyTraceBack_Check(tb));

            // Convert fast locals to f_locals dict -> CPython uses 'fast locals' internally and f_locals is NULL
            // until FastToLocals is called.
            PyFrame_FastToLocals(frame);

            static PyObject* traceback_hide_string = PyUnicode_FromString("__rookout__tracebackhide__");

            if (PyDict_Contains(frame->f_locals, traceback_hide_string) == 1) {
                Py_XINCREF(next);
                Py_XSETREF(*last_not_hidden_link, next);
                prev_link = last_not_hidden_link;
            } else { // don't hide if not found or PyDict_Contains returned error
                prev_link = (PyObject **) &traceback->tb_next;
                last_not_hidden_link = prev_link;
            }

            tb = next;
        }
    done:
        PyErr_Restore(exception, value, base_tb);
    }

    PyObject* CallImportHookRemovingFrames(PyObject* self, PyObject* args, PyObject* kwargs) {
        PyObject* module = NULL;
        if (import_hook == NULL) {
            PyErr_SetString(PyExc_RuntimeError, "This function cannot be called without first calling SetImportHook");
            return NULL;
        }
        module = PyObject_Call(import_hook, args, kwargs);
        if (module != NULL) {
            return module;
        }

        // Got an error - remove any __rookout__tracebackhide__ frames
        if (strip_tracebackhide_frames) {
            remove_tracebackhide_frames();
        }

        return NULL;
    }

    PyObject* SetImportHook(PyObject* self, PyObject* hook) {
        Py_XDECREF(import_hook);
        Py_INCREF(hook);
        import_hook = hook;
        return Py_None;
    }

    PyObject* SetRemoveImportHookFrames(PyObject* self, PyObject* enabled) {
        if (PyObject_IsTrue(enabled)) {
            strip_tracebackhide_frames = true;
        } else {
            strip_tracebackhide_frames = false;
        }

        return Py_None;
    }

}

#endif