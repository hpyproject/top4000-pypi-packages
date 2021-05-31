static CYTHON_INLINE unsigned short __Pyx_PyInt_As_unsigned_short(PyObject *x) {
    const unsigned short neg_one = (unsigned short) ((unsigned short) 0 - (unsigned short) 1), const_zero = (unsigned short) 0;
    const int is_unsigned = neg_one > const_zero;
#if PY_MAJOR_VERSION < 3
    if (likely(PyInt_Check(x))) {
        if (sizeof(unsigned short) < sizeof(long)) {
            __PYX_VERIFY_RETURN_INT(unsigned short, long, PyInt_AS_LONG(x))
        } else {
            long val = PyInt_AS_LONG(x);
            if (is_unsigned && unlikely(val < 0)) {
                goto raise_neg_overflow;
            }
            return (unsigned short) val;
        }
    } else
#endif
    if (likely(PyLong_Check(x))) {
        if (is_unsigned) {
#if CYTHON_USE_PYLONG_INTERNALS
            const digit* digits = ((PyLongObject*)x)->ob_digit;
            switch (Py_SIZE(x)) {
                case  0: return (unsigned short) 0;
                case  1: __PYX_VERIFY_RETURN_INT(unsigned short, digit, digits[0])
                case 2:
                    if (8 * sizeof(unsigned short) > 1 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 2 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, unsigned long, (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) >= 2 * PyLong_SHIFT) {
                            return (unsigned short) (((((unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0]));
                        }
                    }
                    break;
                case 3:
                    if (8 * sizeof(unsigned short) > 2 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 3 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, unsigned long, (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) >= 3 * PyLong_SHIFT) {
                            return (unsigned short) (((((((unsigned short)digits[2]) << PyLong_SHIFT) | (unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0]));
                        }
                    }
                    break;
                case 4:
                    if (8 * sizeof(unsigned short) > 3 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 4 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, unsigned long, (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) >= 4 * PyLong_SHIFT) {
                            return (unsigned short) (((((((((unsigned short)digits[3]) << PyLong_SHIFT) | (unsigned short)digits[2]) << PyLong_SHIFT) | (unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0]));
                        }
                    }
                    break;
            }
#endif
#if CYTHON_COMPILING_IN_CPYTHON
            if (unlikely(Py_SIZE(x) < 0)) {
                goto raise_neg_overflow;
            }
#else
            {
                int result = PyObject_RichCompareBool(x, Py_False, Py_LT);
                if (unlikely(result < 0))
                    return (unsigned short) -1;
                if (unlikely(result == 1))
                    goto raise_neg_overflow;
            }
#endif
            if (sizeof(unsigned short) <= sizeof(unsigned long)) {
                __PYX_VERIFY_RETURN_INT_EXC(unsigned short, unsigned long, PyLong_AsUnsignedLong(x))
#ifdef HAVE_LONG_LONG
            } else if (sizeof(unsigned short) <= sizeof(unsigned PY_LONG_LONG)) {
                __PYX_VERIFY_RETURN_INT_EXC(unsigned short, unsigned PY_LONG_LONG, PyLong_AsUnsignedLongLong(x))
#endif
            }
        } else {
#if CYTHON_USE_PYLONG_INTERNALS
            const digit* digits = ((PyLongObject*)x)->ob_digit;
            switch (Py_SIZE(x)) {
                case  0: return (unsigned short) 0;
                case -1: __PYX_VERIFY_RETURN_INT(unsigned short, sdigit, (sdigit) (-(sdigit)digits[0]))
                case  1: __PYX_VERIFY_RETURN_INT(unsigned short,  digit, +digits[0])
                case -2:
                    if (8 * sizeof(unsigned short) - 1 > 1 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 2 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, long, -(long) (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) - 1 > 2 * PyLong_SHIFT) {
                            return (unsigned short) (((unsigned short)-1)*(((((unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0])));
                        }
                    }
                    break;
                case 2:
                    if (8 * sizeof(unsigned short) > 1 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 2 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, unsigned long, (((((unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) - 1 > 2 * PyLong_SHIFT) {
                            return (unsigned short) ((((((unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0])));
                        }
                    }
                    break;
                case -3:
                    if (8 * sizeof(unsigned short) - 1 > 2 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 3 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, long, -(long) (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) - 1 > 3 * PyLong_SHIFT) {
                            return (unsigned short) (((unsigned short)-1)*(((((((unsigned short)digits[2]) << PyLong_SHIFT) | (unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0])));
                        }
                    }
                    break;
                case 3:
                    if (8 * sizeof(unsigned short) > 2 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 3 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, unsigned long, (((((((unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) - 1 > 3 * PyLong_SHIFT) {
                            return (unsigned short) ((((((((unsigned short)digits[2]) << PyLong_SHIFT) | (unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0])));
                        }
                    }
                    break;
                case -4:
                    if (8 * sizeof(unsigned short) - 1 > 3 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 4 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, long, -(long) (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) - 1 > 4 * PyLong_SHIFT) {
                            return (unsigned short) (((unsigned short)-1)*(((((((((unsigned short)digits[3]) << PyLong_SHIFT) | (unsigned short)digits[2]) << PyLong_SHIFT) | (unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0])));
                        }
                    }
                    break;
                case 4:
                    if (8 * sizeof(unsigned short) > 3 * PyLong_SHIFT) {
                        if (8 * sizeof(unsigned long) > 4 * PyLong_SHIFT) {
                            __PYX_VERIFY_RETURN_INT(unsigned short, unsigned long, (((((((((unsigned long)digits[3]) << PyLong_SHIFT) | (unsigned long)digits[2]) << PyLong_SHIFT) | (unsigned long)digits[1]) << PyLong_SHIFT) | (unsigned long)digits[0])))
                        } else if (8 * sizeof(unsigned short) - 1 > 4 * PyLong_SHIFT) {
                            return (unsigned short) ((((((((((unsigned short)digits[3]) << PyLong_SHIFT) | (unsigned short)digits[2]) << PyLong_SHIFT) | (unsigned short)digits[1]) << PyLong_SHIFT) | (unsigned short)digits[0])));
                        }
                    }
                    break;
            }
#endif
            if (sizeof(unsigned short) <= sizeof(long)) {
                __PYX_VERIFY_RETURN_INT_EXC(unsigned short, long, PyLong_AsLong(x))
#ifdef HAVE_LONG_LONG
            } else if (sizeof(unsigned short) <= sizeof(PY_LONG_LONG)) {
                __PYX_VERIFY_RETURN_INT_EXC(unsigned short, PY_LONG_LONG, PyLong_AsLongLong(x))
#endif
            }
        }
        {
#if (CYTHON_COMPILING_IN_PYPY || CYTHON_COMPILING_IN_LIMITED_API) && !defined(_PyLong_AsByteArray)
            PyErr_SetString(PyExc_RuntimeError,
                            "_PyLong_AsByteArray() not available, cannot convert large numbers");
#else
            unsigned short val;
            PyObject *v = __Pyx_PyNumber_IntOrLong(x);
 #if PY_MAJOR_VERSION < 3
            if (likely(v) && !PyLong_Check(v)) {
                PyObject *tmp = v;
                v = PyNumber_Long(tmp);
                Py_DECREF(tmp);
            }
 #endif
            if (likely(v)) {
                int one = 1; int is_little = (int)*(unsigned char *)&one;
                unsigned char *bytes = (unsigned char *)&val;
                int ret = _PyLong_AsByteArray((PyLongObject *)v,
                                              bytes, sizeof(val),
                                              is_little, !is_unsigned);
                Py_DECREF(v);
                if (likely(!ret))
                    return val;
            }
#endif
            return (unsigned short) -1;
        }
    } else {
        unsigned short val;
        PyObject *tmp = __Pyx_PyNumber_IntOrLong(x);
        if (!tmp) return (unsigned short) -1;
        val = __Pyx_PyInt_As_unsigned_short(tmp);
        Py_DECREF(tmp);
        return val;
    }
raise_overflow:
    PyErr_SetString(PyExc_OverflowError,
        "value too large to convert to unsigned short");
    return (unsigned short) -1;
raise_neg_overflow:
    PyErr_SetString(PyExc_OverflowError,
        "can't convert negative value to unsigned short");
    return (unsigned short) -1;
}

