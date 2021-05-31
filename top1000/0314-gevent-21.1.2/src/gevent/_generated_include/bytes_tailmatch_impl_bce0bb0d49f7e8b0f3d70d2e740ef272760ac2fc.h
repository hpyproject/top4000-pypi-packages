static int __Pyx_PyBytes_SingleTailmatch(PyObject* self, PyObject* arg,
                                         Py_ssize_t start, Py_ssize_t end, int direction) {
    const char* self_ptr = PyBytes_AS_STRING(self);
    Py_ssize_t self_len = PyBytes_GET_SIZE(self);
    const char* sub_ptr;
    Py_ssize_t sub_len;
    int retval;
    Py_buffer view;
    view.obj = NULL;
    if ( PyBytes_Check(arg) ) {
        sub_ptr = PyBytes_AS_STRING(arg);
        sub_len = PyBytes_GET_SIZE(arg);
    }
#if PY_MAJOR_VERSION < 3
    else if ( PyUnicode_Check(arg) ) {
        return (int) PyUnicode_Tailmatch(self, arg, start, end, direction);
    }
#endif
    else {
        if (unlikely(PyObject_GetBuffer(self, &view, PyBUF_SIMPLE) == -1))
            return -1;
        sub_ptr = (const char*) view.buf;
        sub_len = view.len;
    }
    if (end > self_len)
        end = self_len;
    else if (end < 0)
        end += self_len;
    if (end < 0)
        end = 0;
    if (start < 0)
        start += self_len;
    if (start < 0)
        start = 0;
    if (direction > 0) {
        if (end-sub_len > start)
            start = end - sub_len;
    }
    if (start + sub_len <= end)
        retval = !memcmp(self_ptr+start, sub_ptr, (size_t)sub_len);
    else
        retval = 0;
    if (view.obj)
        PyBuffer_Release(&view);
    return retval;
}
static int __Pyx_PyBytes_TailmatchTuple(PyObject* self, PyObject* substrings,
                                        Py_ssize_t start, Py_ssize_t end, int direction) {
    Py_ssize_t i, count = PyTuple_GET_SIZE(substrings);
    for (i = 0; i < count; i++) {
        int result;
#if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        result = __Pyx_PyBytes_SingleTailmatch(self, PyTuple_GET_ITEM(substrings, i),
                                               start, end, direction);
#else
        PyObject* sub = PySequence_ITEM(substrings, i);
        if (unlikely(!sub)) return -1;
        result = __Pyx_PyBytes_SingleTailmatch(self, sub, start, end, direction);
        Py_DECREF(sub);
#endif
        if (result) {
            return result;
        }
    }
    return 0;
}
static int __Pyx_PyBytes_Tailmatch(PyObject* self, PyObject* substr,
                                   Py_ssize_t start, Py_ssize_t end, int direction) {
    if (unlikely(PyTuple_Check(substr))) {
        return __Pyx_PyBytes_TailmatchTuple(self, substr, start, end, direction);
    }
    return __Pyx_PyBytes_SingleTailmatch(self, substr, start, end, direction);
}

