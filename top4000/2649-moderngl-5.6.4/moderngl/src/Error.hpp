#pragma once

#include "Python.hpp"

#define MGLError_Set(...) MGLError_SetTrace(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

void MGLError_SetTrace(const char * filename, const char * function, int line, const char * format, ...);
void MGLError_SetTrace(const char * filename, const char * function, int line, PyObject * message);
