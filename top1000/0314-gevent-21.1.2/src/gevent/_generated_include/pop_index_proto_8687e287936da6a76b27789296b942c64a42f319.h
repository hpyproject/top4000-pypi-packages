static PyObject* __Pyx__PyObject_PopNewIndex(PyObject* L, PyObject* py_ix);
static PyObject* __Pyx__PyObject_PopIndex(PyObject* L, PyObject* py_ix);
#if CYTHON_USE_PYLIST_INTERNALS && CYTHON_ASSUME_SAFE_MACROS
static PyObject* __Pyx__PyList_PopIndex(PyObject* L, PyObject* py_ix, Py_ssize_t ix);
#define __Pyx_PyObject_PopIndex(L, py_ix, ix, is_signed, type, to_py_func) (\
    (likely(PyList_CheckExact(L) && __Pyx_fits_Py_ssize_t(ix, type, is_signed))) ?\
        __Pyx__PyList_PopIndex(L, py_ix, ix) : (\
        (unlikely((py_ix) == Py_None)) ? __Pyx__PyObject_PopNewIndex(L, to_py_func(ix)) :\
            __Pyx__PyObject_PopIndex(L, py_ix)))
#define __Pyx_PyList_PopIndex(L, py_ix, ix, is_signed, type, to_py_func) (\
    __Pyx_fits_Py_ssize_t(ix, type, is_signed) ?\
        __Pyx__PyList_PopIndex(L, py_ix, ix) : (\
        (unlikely((py_ix) == Py_None)) ? __Pyx__PyObject_PopNewIndex(L, to_py_func(ix)) :\
            __Pyx__PyObject_PopIndex(L, py_ix)))
#else
#define __Pyx_PyList_PopIndex(L, py_ix, ix, is_signed, type, to_py_func)\
    __Pyx_PyObject_PopIndex(L, py_ix, ix, is_signed, type, to_py_func)
#define __Pyx_PyObject_PopIndex(L, py_ix, ix, is_signed, type, to_py_func) (\
    (unlikely((py_ix) == Py_None)) ? __Pyx__PyObject_PopNewIndex(L, to_py_func(ix)) :\
        __Pyx__PyObject_PopIndex(L, py_ix))
#endif

