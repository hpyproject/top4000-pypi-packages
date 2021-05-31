#include "Error.hpp"

#include <cstdarg>

void MGLError_SetTrace(const char * filename, const char * function, int line, const char * format, ...) {
	// MGLError * error = (MGLError *)MGLError_tp_new(&MGLError_Type, 0, 0);

	PyObject * moderngl = PyImport_ImportModule("moderngl");
	if (!moderngl) {
		PyErr_Clear();
		return;
	}

	PyObject * error_type = PyObject_GetAttrString(moderngl, "Error");
	if (!error_type) {
		return;
	}

	va_list va_args;
	va_start(va_args, format);
	PyObject * message = PyUnicode_FromFormatV(format, va_args);
	va_end(va_args);

	// PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
	PyErr_Format(error_type, "%s", PyUnicode_AsUTF8(message));
	Py_DECREF(error_type);
	Py_DECREF(message);
}

void MGLError_SetTrace(const char * filename, const char * function, int line, PyObject * message) {
	// MGLError * error = (MGLError *)MGLError_tp_new(&MGLError_Type, 0, 0);

	PyObject * moderngl = PyImport_ImportModule("moderngl");
	if (!moderngl) {
		PyErr_Clear();
		return;
	}

	PyObject * error_type = PyObject_GetAttrString(moderngl, "Error");
	if (!error_type) {
		return;
	}

	// error->filename = filename;
	// error->function = function;
	// error->line = line;

	// error->args = PyTuple_New(1);
	// PyTuple_SET_ITEM(error->args, 0, message);

	// PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
	PyErr_Format(error_type, "%s", PyUnicode_AsUTF8(message));
	Py_DECREF(error_type);
	Py_DECREF(message);
}
