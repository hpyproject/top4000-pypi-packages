#ifndef PyObjC_OPTION_H
#define PyObjC_OPTION_H

extern BOOL PyObjC_Verbose;
extern BOOL PyObjC_UseKVO;
extern BOOL PyObjCPointer_RaiseException;
extern BOOL PyObjC_StructsIndexable;
extern BOOL PyObjC_StructsWritable;

extern int        PyObjC_DeprecationVersion;
extern Py_ssize_t PyObjC_MappingCount;
extern int        PyObjC_NSCoding_Version;
extern PyObject*  PyObjC_Encoder;
extern PyObject*  PyObjC_Decoder;
extern PyObject*  PyObjC_CopyFunc;
extern PyObject*  PyObjC_ClassExtender;
extern PyObject*  PyObjC_MakeBundleForClass;
extern PyObject*  PyObjC_NSNumberWrapper;
extern PyObject*  PyObjC_CallableDocFunction;
extern PyObject*  PyObjC_CallableSignatureFunction;

extern PyObject* PyObjC_DictLikeTypes;
extern PyObject* PyObjC_ListLikeTypes;
extern PyObject* PyObjC_SetLikeTypes;
extern PyObject* PyObjC_DateLikeTypes;

extern int PyObjC_SetupOptions(PyObject* m);

#endif /* PyObjC_OPTION_H */
