#if CYTHON_COMPILING_IN_CPYTHON
static CYTHON_INLINE void __Pyx_crop_slice(Py_ssize_t* _start, Py_ssize_t* _stop, Py_ssize_t* _length) {
    Py_ssize_t start = *_start, stop = *_stop, length = *_length;
    if (start < 0) {
        start += length;
        if (start < 0)
            start = 0;
    }
    if (stop < 0)
        stop += length;
    else if (stop > length)
        stop = length;
    *_length = stop - start;
    *_start = start;
    *_stop = stop;
}
static CYTHON_INLINE PyObject* __Pyx_PyList_GetSlice(
            PyObject* src, Py_ssize_t start, Py_ssize_t stop) {
    Py_ssize_t length = PyList_GET_SIZE(src);
    __Pyx_crop_slice(&start, &stop, &length);
    return __Pyx_PyList_FromArray(((PyListObject*)src)->ob_item + start, length);
}
static CYTHON_INLINE PyObject* __Pyx_PyTuple_GetSlice(
            PyObject* src, Py_ssize_t start, Py_ssize_t stop) {
    Py_ssize_t length = PyTuple_GET_SIZE(src);
    __Pyx_crop_slice(&start, &stop, &length);
    return __Pyx_PyTuple_FromArray(((PyTupleObject*)src)->ob_item + start, length);
}
#endif

