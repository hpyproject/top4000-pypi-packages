static int __Pyx_PyUnicode_TailmatchTuple(PyObject* s, PyObject* substrings,
                                          Py_ssize_t start, Py_ssize_t end, int direction) {
    Py_ssize_t i, count = PyTuple_GET_SIZE(substrings);
    for (i = 0; i < count; i++) {
        Py_ssize_t result;
#if CYTHON_ASSUME_SAFE_MACROS && !CYTHON_AVOID_BORROWED_REFS
        result = PyUnicode_Tailmatch(s, PyTuple_GET_ITEM(substrings, i),
                                     start, end, direction);
#else
        PyObject* sub = PySequence_ITEM(substrings, i);
        if (unlikely(!sub)) return -1;
        result = PyUnicode_Tailmatch(s, sub, start, end, direction);
        Py_DECREF(sub);
#endif
        if (result) {
            return (int) result;
        }
    }
    return 0;
}
static int __Pyx_PyUnicode_Tailmatch(PyObject* s, PyObject* substr,
                                     Py_ssize_t start, Py_ssize_t end, int direction) {
    if (unlikely(PyTuple_Check(substr))) {
        return __Pyx_PyUnicode_TailmatchTuple(s, substr, start, end, direction);
    }
    return (int) PyUnicode_Tailmatch(s, substr, start, end, direction);
}

