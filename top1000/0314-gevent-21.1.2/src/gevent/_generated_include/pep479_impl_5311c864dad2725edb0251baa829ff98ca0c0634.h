static void __Pyx_Generator_Replace_StopIteration(CYTHON_UNUSED int in_async_gen) {
    PyObject *exc, *val, *tb, *cur_exc;
    __Pyx_PyThreadState_declare
    #ifdef __Pyx_StopAsyncIteration_USED
    int is_async_stopiteration = 0;
    #endif
    cur_exc = PyErr_Occurred();
    if (likely(!__Pyx_PyErr_GivenExceptionMatches(cur_exc, PyExc_StopIteration))) {
        #ifdef __Pyx_StopAsyncIteration_USED
        if (in_async_gen && unlikely(__Pyx_PyErr_GivenExceptionMatches(cur_exc, __Pyx_PyExc_StopAsyncIteration))) {
            is_async_stopiteration = 1;
        } else
        #endif
            return;
    }
    __Pyx_PyThreadState_assign
    __Pyx_GetException(&exc, &val, &tb);
    Py_XDECREF(exc);
    Py_XDECREF(val);
    Py_XDECREF(tb);
    PyErr_SetString(PyExc_RuntimeError,
        #ifdef __Pyx_StopAsyncIteration_USED
        is_async_stopiteration ? "async generator raised StopAsyncIteration" :
        in_async_gen ? "async generator raised StopIteration" :
        #endif
        "generator raised StopIteration");
}

