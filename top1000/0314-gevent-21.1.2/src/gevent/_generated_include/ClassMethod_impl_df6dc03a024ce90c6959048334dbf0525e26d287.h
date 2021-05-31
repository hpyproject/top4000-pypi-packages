static PyObject* __Pyx_Method_ClassMethod(PyObject *method) {
#if CYTHON_COMPILING_IN_PYPY && PYPY_VERSION_NUM <= 0x05080000
    if (PyObject_TypeCheck(method, &PyWrapperDescr_Type)) {
        return PyClassMethod_New(method);
    }
#else
#if CYTHON_COMPILING_IN_PYSTON || CYTHON_COMPILING_IN_PYPY
    if (PyMethodDescr_Check(method))
#else
    #if PY_MAJOR_VERSION == 2
    static PyTypeObject *methoddescr_type = NULL;
    if (unlikely(methoddescr_type == NULL)) {
       PyObject *meth = PyObject_GetAttrString((PyObject*)&PyList_Type, "append");
       if (unlikely(!meth)) return NULL;
       methoddescr_type = Py_TYPE(meth);
       Py_DECREF(meth);
    }
    #else
    PyTypeObject *methoddescr_type = &PyMethodDescr_Type;
    #endif
    if (__Pyx_TypeCheck(method, methoddescr_type))
#endif
    {
        PyMethodDescrObject *descr = (PyMethodDescrObject *)method;
        #if PY_VERSION_HEX < 0x03020000
        PyTypeObject *d_type = descr->d_type;
        #else
        PyTypeObject *d_type = descr->d_common.d_type;
        #endif
        return PyDescr_NewClassMethod(d_type, descr->d_method);
    }
#endif
    else if (PyMethod_Check(method)) {
        return PyClassMethod_New(PyMethod_GET_FUNCTION(method));
    }
    else {
        return PyClassMethod_New(method);
    }
}

