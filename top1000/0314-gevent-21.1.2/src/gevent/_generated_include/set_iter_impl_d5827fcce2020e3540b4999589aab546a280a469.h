static CYTHON_INLINE PyObject* __Pyx_set_iterator(PyObject* iterable, int is_set,
                                                  Py_ssize_t* p_orig_length, int* p_source_is_set) {
#if CYTHON_COMPILING_IN_CPYTHON
    is_set = is_set || likely(PySet_CheckExact(iterable) || PyFrozenSet_CheckExact(iterable));
    *p_source_is_set = is_set;
    if (likely(is_set)) {
        *p_orig_length = PySet_Size(iterable);
        Py_INCREF(iterable);
        return iterable;
    }
#else
    (void)is_set;
    *p_source_is_set = 0;
#endif
    *p_orig_length = 0;
    return PyObject_GetIter(iterable);
}
static CYTHON_INLINE int __Pyx_set_iter_next(
        PyObject* iter_obj, Py_ssize_t orig_length,
        Py_ssize_t* ppos, PyObject **value,
        int source_is_set) {
    if (!CYTHON_COMPILING_IN_CPYTHON || unlikely(!source_is_set)) {
        *value = PyIter_Next(iter_obj);
        if (unlikely(!*value)) {
            return __Pyx_IterFinish();
        }
        (void)orig_length;
        (void)ppos;
        return 1;
    }
#if CYTHON_COMPILING_IN_CPYTHON
    if (unlikely(PySet_GET_SIZE(iter_obj) != orig_length)) {
        PyErr_SetString(
            PyExc_RuntimeError,
            "set changed size during iteration");
        return -1;
    }
    {
        Py_hash_t hash;
        int ret = _PySet_NextEntry(iter_obj, ppos, value, &hash);
        assert (ret != -1);
        if (likely(ret)) {
            Py_INCREF(*value);
            return 1;
        }
    }
#endif
    return 0;
}

