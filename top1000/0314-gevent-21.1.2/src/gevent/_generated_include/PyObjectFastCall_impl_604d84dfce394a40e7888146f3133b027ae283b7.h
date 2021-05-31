static PyObject* __Pyx_PyObject_FastCall_fallback(PyObject *func, PyObject **args, Py_ssize_t nargs, PyObject *kwargs) {
    PyObject *argstuple;
    PyObject *result;
    Py_ssize_t i;
    argstuple = PyTuple_New(nargs);
    if (unlikely(!argstuple)) return NULL;
    for (i = 0; i < nargs; i++) {
        Py_INCREF(args[i]);
        PyTuple_SET_ITEM(argstuple, i, args[i]);
    }
    result = __Pyx_PyObject_Call(func, argstuple, kwargs);
    Py_DECREF(argstuple);
    return result;
}
static CYTHON_INLINE PyObject* __Pyx_PyObject_FastCallDict(PyObject *func, PyObject **args, Py_ssize_t _nargs, PyObject *kwargs) {
    Py_ssize_t nargs = __Pyx_PyVectorcall_NARGS(_nargs);
#if CYTHON_COMPILING_IN_CPYTHON
    if (nargs == 0 && kwargs == NULL) {
#ifdef __Pyx_CyFunction_USED
        if (__Pyx_IsCyOrPyCFunction(func))
#else
        if (PyCFunction_Check(func))
#endif
        {
            if (likely(PyCFunction_GET_FLAGS(func) & METH_NOARGS)) {
                return __Pyx_PyObject_CallMethO(func, NULL);
            }
        }
    }
    else if (nargs == 1 && kwargs == NULL) {
        if (PyCFunction_Check(func))
        {
            if (likely(PyCFunction_GET_FLAGS(func) & METH_O)) {
                return __Pyx_PyObject_CallMethO(func, args[0]);
            }
        }
    }
#endif
    #if PY_VERSION_HEX < 0x030800B1
    #if CYTHON_FAST_PYCCALL
    if (PyCFunction_Check(func)) {
        if (kwargs) {
            return _PyCFunction_FastCallDict(func, args, nargs, kwargs);
        } else {
            return _PyCFunction_FastCallKeywords(func, args, nargs, NULL);
        }
    }
    #if PY_VERSION_HEX >= 0x030700A1
    if (!kwargs && __Pyx_IS_TYPE(func, &PyMethodDescr_Type)) {
        return _PyMethodDescr_FastCallKeywords(func, args, nargs, NULL);
    }
    #endif
    #endif
    #if CYTHON_FAST_PYCALL
    if (PyFunction_Check(func)) {
        return __Pyx_PyFunction_FastCallDict(func, args, nargs, kwargs);
    }
    #endif
    #endif
    #if CYTHON_VECTORCALL
    vectorcallfunc f = _PyVectorcall_Function(func);
    if (f) {
        return f(func, args, nargs, kwargs);
    }
    #elif defined(__Pyx_CyFunction_USED) && CYTHON_BACKPORT_VECTORCALL
    if (__Pyx_CyFunction_CheckExact(func)) {
        __pyx_vectorcallfunc f = __Pyx_CyFunction_func_vectorcall(func);
        if (f) return f(func, args, nargs, kwargs);
    }
    #endif
    if (nargs == 0) {
        return __Pyx_PyObject_Call(func, __pyx_empty_tuple, kwargs);
    }
    return __Pyx_PyObject_FastCall_fallback(func, args, nargs, kwargs);
}

