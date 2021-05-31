/*############################################################################
#
# Copyright (c) 2004 Zope Foundation and Contributors.
# All Rights Reserved.
#
# This software is subject to the provisions of the Zope Public License,
# Version 2.1 (ZPL).  A copy of the ZPL should accompany this distribution.
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY AND ALL EXPRESS OR IMPLIED
# WARRANTIES ARE DISCLAIMED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF TITLE, MERCHANTABILITY, AGAINST INFRINGEMENT, AND FITNESS
# FOR A PARTICULAR PURPOSE.
#
############################################################################*/

#include "Python.h"


#if PY_MAJOR_VERSION >= 3
  #define MOD_ERROR_VAL NULL
#else
  #define MOD_ERROR_VAL
#endif

typedef struct {
  PyUnicodeObject base;
  PyObject *domain;
  PyObject *default_;
  PyObject *mapping;
  PyObject *value_plural;
  PyObject *default_plural;
  PyObject *number;
} Message;

static PyTypeObject MessageType;

static PyObject *
Message_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"value", "domain", "default", "mapping",
                           "msgid_plural", "default_plural", "number", NULL};
  PyObject *value, *domain=NULL, *default_=NULL, *mapping=NULL, *s;
  PyObject *value_plural=NULL, *default_plural=NULL, *number=NULL;
  Message *self;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OOOOOO", kwlist,
                                   &value, &domain, &default_, &mapping,
                                   &value_plural, &default_plural, &number))
    return NULL;

  if (number != NULL && Py_None != number) {
#if PY_MAJOR_VERSION >= 3
    if (!(PyLong_Check(number) || PyFloat_Check(number))) {
#else
    if (!(PyLong_Check(number) || PyInt_Check(number) || PyFloat_Check(number))) {
#endif
      PyErr_SetString(PyExc_TypeError,
                      "`number` should be an integer or a float");
      return NULL;
    }
  }

  args = Py_BuildValue("(O)", value);
  if (args == NULL)
    return NULL;

  s = PyUnicode_Type.tp_new(type, args, NULL);
  Py_DECREF(args);
  if (s == NULL)
    return NULL;

  if (!PyObject_TypeCheck(s, &MessageType)) {
    PyErr_SetString(PyExc_TypeError, "unicode.__new__ didn't return a Message");
    Py_DECREF(s);
    return NULL;
  }

  self = (Message*)s;

  if (PyObject_TypeCheck(value, &MessageType)) {
    /* value is a Message so we copy it and use it as base */
    self->domain = ((Message *)value)->domain;
    self->default_ = ((Message *)value)->default_;
    self->mapping = ((Message *)value)->mapping;
    self->value_plural = ((Message *)value)->value_plural;
    self->default_plural = ((Message *)value)->default_plural;
    self->number = ((Message *)value)->number;
  }
  else {
    self->domain = NULL;
    self->default_ = NULL;
    self->mapping = NULL;
    self->value_plural = NULL;
    self->default_plural = NULL;
    self->number = NULL;
  }

  if (domain != NULL)
    self->domain = domain;

  if (default_ != NULL)
    self->default_ = default_;

  if (mapping != NULL)
    self->mapping = mapping;

  if (value_plural != NULL)
    self->value_plural = value_plural;

  if (default_plural != NULL)
    self->default_plural = default_plural;

  if (number != NULL) {
    self->number = number;
  }

  Py_XINCREF(self->mapping);
  Py_XINCREF(self->default_);
  Py_XINCREF(self->domain);
  Py_XINCREF(self->value_plural);
  Py_XINCREF(self->default_plural);
  Py_XINCREF(self->number);

  return (PyObject *)self;
}

/* Code to access structure members by accessing attributes */

#include "structmember.h"

static PyMemberDef Message_members[] = {
  { "domain", T_OBJECT, offsetof(Message, domain), READONLY },
  { "default", T_OBJECT, offsetof(Message, default_), READONLY },
  { "mapping", T_OBJECT, offsetof(Message, mapping), READONLY },
  { "msgid_plural", T_OBJECT, offsetof(Message, value_plural), READONLY },
  { "default_plural", T_OBJECT, offsetof(Message, default_plural), READONLY },
  { "number", T_OBJECT, offsetof(Message, number), READONLY },
  {NULL}    /* Sentinel */
};

static int
Message_traverse(Message *self, visitproc visit, void *arg)
{
  Py_VISIT(self->domain);
  Py_VISIT(self->default_);
  Py_VISIT(self->mapping);
  Py_VISIT(self->value_plural);
  Py_VISIT(self->default_plural);
  Py_VISIT(self->number);
  return 0;
}

static int
Message_clear(Message *self)
{
  Py_CLEAR(self->domain);
  Py_CLEAR(self->default_);
  Py_CLEAR(self->mapping);
  Py_CLEAR(self->value_plural);
  Py_CLEAR(self->default_plural);
  Py_CLEAR(self->number);
  return 0;
}

static void
Message_dealloc(Message *self)
{
  PyObject_GC_UnTrack((PyObject *)self);
  Message_clear(self);
  PyUnicode_Type.tp_dealloc((PyObject*)self);
}

static PyObject *
Message_reduce(Message *self)
{
  PyObject *value, *result;
  value = PyObject_CallFunctionObjArgs((PyObject *)&PyUnicode_Type, self, NULL);
  if (value == NULL)
    return NULL;
  result = Py_BuildValue("(O(OOOOOOO))", Py_TYPE(&(self->base)),
             value,
             self->domain ? self->domain : Py_None,
             self->default_ ? self->default_ : Py_None,
             self->mapping ? self->mapping : Py_None,
             self->value_plural ? self->value_plural : Py_None,
             self->default_plural ? self->default_plural : Py_None,
             self->number ? self->number : Py_None);
  Py_DECREF(value);
  return result;
}

static PyMethodDef Message_methods[] = {
  {"__reduce__", (PyCFunction)Message_reduce, METH_NOARGS,
   "Reduce messages to a serializable form."},
  {NULL}  /* Sentinel */
};


static char MessageType__doc__[] =
"Message\n"
"\n"
"This is a string used as a message.  It has a domain attribute that is\n"
"its source domain, and a default attribute that is its default text to\n"
"display when there is no translation.  domain may be None meaning there is\n"
"no translation domain.  default may also be None, in which case the\n"
"message id itself implicitly serves as the default text.\n";

static PyTypeObject
MessageType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /* tp_name           */ "zope.i18nmessageid.message."
                                "Message",
    /* tp_basicsize      */ sizeof(Message),
    /* tp_itemsize       */ 0,
    /* tp_dealloc        */ (destructor)&Message_dealloc,
    /* tp_print          */ (printfunc)0,
    /* tp_getattr        */ (getattrfunc)0,
    /* tp_setattr        */ (setattrfunc)0,
    /* tp_compare        */ 0,
    /* tp_repr           */ (reprfunc)0,
    /* tp_as_number      */ 0,
    /* tp_as_sequence    */ 0,
    /* tp_as_mapping     */ 0,
    /* tp_hash           */ (hashfunc)0,
    /* tp_call           */ (ternaryfunc)0,
    /* tp_str            */ (reprfunc)0,
    /* tp_getattro       */ (getattrofunc)0,
    /* tp_setattro       */ (setattrofunc)0,
    /* tp_as_buffer      */ 0,
    /* tp_flags          */ Py_TPFLAGS_DEFAULT
                            | Py_TPFLAGS_BASETYPE
                            | Py_TPFLAGS_HAVE_GC,
    /* tp_doc            */ MessageType__doc__,
    /* tp_traverse       */ (traverseproc)Message_traverse,
    /* tp_clear          */ (inquiry)Message_clear,
    /* tp_richcompare    */ (richcmpfunc)0,
    /* tp_weaklistoffset */ (long)0,
    /* tp_iter           */ (getiterfunc)0,
    /* tp_iternext       */ (iternextfunc)0,
    /* tp_methods        */ Message_methods,
    /* tp_members        */ Message_members,
    /* tp_getset         */ 0,
    /* tp_base           */ 0,
    /* tp_dict           */ 0, /* internal use */
    /* tp_descr_get      */ (descrgetfunc)0,
    /* tp_descr_set      */ (descrsetfunc)0,
    /* tp_dictoffset     */ 0,
    /* tp_init           */ (initproc)0,
    /* tp_alloc          */ (allocfunc)0,
    /* tp_new            */ (newfunc)Message_new,
    /* tp_free           */ 0, /* Low-level free-mem routine */
    /* tp_is_gc          */ (inquiry)0, /* For PyObject_IS_GC */
};

/* End of code for Message objects */
/* -------------------------------------------------------- */


/* List of methods defined in the module */
static struct PyMethodDef _zope_i18nmessageid_message_methods[] = {
  {NULL, (PyCFunction)NULL, 0, NULL},         /* sentinel */
};

static char _zope_i18nmessageid_message_module_name[] =
"_zope_i18nmessageid_message";

static char _zope_i18nmessageid_message_module_documentation[] =
"I18n Messages";

#if PY_MAJOR_VERSION >= 3
  static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    _zope_i18nmessageid_message_module_name,/* m_name */
    _zope_i18nmessageid_message_module_documentation,/* m_doc */
    -1,/* m_size */
    _zope_i18nmessageid_message_methods,/* m_methods */
    NULL,/* m_reload */
    NULL,/* m_traverse */
    NULL,/* m_clear */
    NULL,/* m_free */
  };
#endif

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
  #define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
#if PY_MAJOR_VERSION >= 3
 PyInit__zope_i18nmessageid_message(void)
#else
 init_zope_i18nmessageid_message(void)
#endif
{
  PyObject *m;
  /* Initialize types: */
  MessageType.tp_base = &PyUnicode_Type;
  if (PyType_Ready(&MessageType) < 0)
    return MOD_ERROR_VAL;

  /* Create the module and add the functions */
#if PY_MAJOR_VERSION >= 3
  m = PyModule_Create(&moduledef);
#else
  m = Py_InitModule3(_zope_i18nmessageid_message_module_name,
                     _zope_i18nmessageid_message_methods,
                     _zope_i18nmessageid_message_module_documentation);
#endif

  if (m == NULL)
    return MOD_ERROR_VAL;

  /* Add types: */
  if (PyModule_AddObject(m, "Message", (PyObject *)&MessageType) < 0)
    return MOD_ERROR_VAL;

#if PY_MAJOR_VERSION >= 3
  return m;
#endif

}
