// Ensure that Python.h is included before any other header.
#include "common.h"

#ifndef PYPY_VERSION
namespace native_extensions {

PyObject* CallImportHookRemovingFrames(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* SetImportHook(PyObject* self, PyObject* hook);

PyObject* SetRemoveImportHookFrames(PyObject* self, PyObject* enabled);

}  // namespace native_extensions
#endif