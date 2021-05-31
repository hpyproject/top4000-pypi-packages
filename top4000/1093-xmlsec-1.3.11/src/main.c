// Copyright (c) 2017 Ryan Leckey
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "common.h"
#include "platform.h"
#include "exception.h"

#include <xmlsec/xmlsec.h>
#include <xmlsec/crypto.h>
#include <xmlsec/errors.h>
#include <xmlsec/base64.h>

#define _PYXMLSEC_FREE_NONE 0
#define _PYXMLSEC_FREE_XMLSEC 1
#define _PYXMLSEC_FREE_CRYPTOLIB 2
#define _PYXMLSEC_FREE_ALL 3

static int free_mode = _PYXMLSEC_FREE_NONE;

#define MODULE_DOC "The tiny python wrapper around xmlsec1 (" XMLSEC_VERSION ") library"

#ifndef XMLSEC_NO_CRYPTO_DYNAMIC_LOADING
static const xmlChar* PyXmlSec_GetCryptoLibName() {
#if XMLSEC_VERSION_HEX > 0x10214
    // xmlSecGetDefaultCrypto was introduced in version 1.2.21
    const xmlChar* cryptoLib = xmlSecGetDefaultCrypto();
#else
    const xmlChar* cryptoLib = (const xmlChar*) XMLSEC_CRYPTO;
#endif
    PYXMLSEC_DEBUGF("dynamic crypto library: %s", cryptoLib);
    return cryptoLib;
}
#endif // !XMLSEC_NO_CRYPTO_DYNAMIC_LOADING

static void PyXmlSec_Free(int what) {
    PYXMLSEC_DEBUGF("free resources %d", what);
    switch (what) {
    case _PYXMLSEC_FREE_ALL:
        xmlSecCryptoAppShutdown();
    case _PYXMLSEC_FREE_CRYPTOLIB:
#ifndef XMLSEC_NO_CRYPTO_DYNAMIC_LOADING
        xmlSecCryptoDLUnloadLibrary(PyXmlSec_GetCryptoLibName());
#endif
    case _PYXMLSEC_FREE_XMLSEC:
        xmlSecShutdown();
    }
    free_mode = _PYXMLSEC_FREE_NONE;
}

static int PyXmlSec_Init(void) {
    if (xmlSecInit() < 0) {
        PyXmlSec_SetLastError("cannot initialize xmlsec library.");
        PyXmlSec_Free(_PYXMLSEC_FREE_NONE);
        return -1;
    }

    if (xmlSecCheckVersion() != 1) {
        PyXmlSec_SetLastError("xmlsec library version mismatch.");
        PyXmlSec_Free(_PYXMLSEC_FREE_XMLSEC);
        return -1;
    }

#ifndef XMLSEC_NO_CRYPTO_DYNAMIC_LOADING
    if (xmlSecCryptoDLLoadLibrary(PyXmlSec_GetCryptoLibName()) < 0) {
        PyXmlSec_SetLastError("cannot load crypto library for xmlsec.");
        PyXmlSec_Free(_PYXMLSEC_FREE_XMLSEC);
        return -1;
    }
#endif /* XMLSEC_CRYPTO_DYNAMIC_LOADING */

  /* Init crypto library */
    if (xmlSecCryptoAppInit(NULL) < 0) {
        PyXmlSec_SetLastError("cannot initialize crypto library application.");
        PyXmlSec_Free(_PYXMLSEC_FREE_CRYPTOLIB);
        return -1;
    }

  /* Init xmlsec-crypto library */
    if (xmlSecCryptoInit() < 0) {
        PyXmlSec_SetLastError("cannot initialize crypto library.");
        PyXmlSec_Free(_PYXMLSEC_FREE_ALL);
        return -1;
    }
    free_mode = _PYXMLSEC_FREE_ALL;
    return 0;
}

static char PyXmlSec_PyInit__doc__[] = \
    "init() -> None\n"
    "Initializes the library for general operation.\n\n"
    "This is called upon library import and does not need to be called\n"
    "again :func:`~.shutdown` is called explicitly).\n";
static PyObject* PyXmlSec_PyInit(PyObject *self) {
   if (PyXmlSec_Init() < 0) {
        return NULL;
   }
   Py_RETURN_NONE;
}

static char PyXmlSec_PyShutdown__doc__[] = \
    "shutdown() -> None\n"
    "Shutdowns the library and cleanup any leftover resources.\n\n"
    "This is called automatically upon interpreter termination and\n"
    "should not need to be called explicitly.";
static PyObject* PyXmlSec_PyShutdown(PyObject* self) {
    PyXmlSec_Free(free_mode);
    Py_RETURN_NONE;
}

static char PyXmlSec_PyEnableDebugOutput__doc__[] = \
    "enable_debug_trace(enabled) -> None\n"
    "Enables or disables calling LibXML2 callback from the default errors callback.\n\n"
    ":param enabled: flag, debug trace is enabled or disabled\n"
    ":type enabled: :class:`bool`";
static PyObject* PyXmlSec_PyEnableDebugOutput(PyObject *self, PyObject* args, PyObject* kwargs) {
    static char *kwlist[] = { "enabled", NULL};
    PyObject* enabled = Py_True;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O:enable_debug_trace", kwlist, &enabled)) {
        return NULL;
    }
    PyXmlSecEnableDebugTrace(PyObject_IsTrue(enabled));
    Py_RETURN_NONE;
}

static char PyXmlSec_PyBase64DefaultLineSize__doc__[] = \
    "base64_default_line_size(size = None)\n"
    "Configures the default maximum columns size for base64 encoding.\n\n"
    "If ``size`` is not given, this function returns the current default size, acting as a getter. "
    "If ``size`` is given, a new value is applied and this function returns nothing, acting as a setter.\n"
    ":param size: new default size value (optional)\n"
    ":type size: :class:`int` or :data:`None`";
static PyObject* PyXmlSec_PyBase64DefaultLineSize(PyObject *self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = { "size", NULL };
    PyObject *pySize = NULL;
    int size;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O:base64_default_line_size", kwlist, &pySize)) {
        return NULL;
    }
    if (pySize == NULL) {
        return PyLong_FromLong(xmlSecBase64GetDefaultLineSize());
    }
    size = (int)PyLong_AsLong(pySize);
    if (PyErr_Occurred()) {
        return NULL;
    }
    if (size < 0) {
        PyErr_SetString(PyExc_ValueError, "size must be positive");
        return NULL;
    }
    xmlSecBase64SetDefaultLineSize(size);
    Py_RETURN_NONE;
}

static PyMethodDef PyXmlSec_MainMethods[] = {
    {
        "init",
        (PyCFunction)PyXmlSec_PyInit,
        METH_NOARGS,
        PyXmlSec_PyInit__doc__
    },
    {
        "shutdown",
        (PyCFunction)PyXmlSec_PyShutdown,
        METH_NOARGS,
        PyXmlSec_PyShutdown__doc__
    },
    {
        "enable_debug_trace",
        (PyCFunction)PyXmlSec_PyEnableDebugOutput,
        METH_VARARGS|METH_KEYWORDS,
        PyXmlSec_PyEnableDebugOutput__doc__
    },
    {
        "base64_default_line_size",
        (PyCFunction)PyXmlSec_PyBase64DefaultLineSize,
        METH_VARARGS|METH_KEYWORDS,
        PyXmlSec_PyBase64DefaultLineSize__doc__
    },
    {NULL, NULL} /* sentinel */
};

// modules entry points
// loads lxml module
int PyXmlSec_InitLxmlModule(void);
// constants
int PyXmlSec_ConstantsModule_Init(PyObject* package);
// exceptions
int PyXmlSec_ExceptionsModule_Init(PyObject* package);
// keys management
int PyXmlSec_KeyModule_Init(PyObject* package);
// init lxml.tree integration
int PyXmlSec_TreeModule_Init(PyObject* package);
// digital signature management
int PyXmlSec_DSModule_Init(PyObject* package);
// encryption management
int PyXmlSec_EncModule_Init(PyObject* package);
// templates management
int PyXmlSec_TemplateModule_Init(PyObject* package);

static int PyXmlSec_PyClear(PyObject *self) {
    PyXmlSec_Free(free_mode);
    return 0;
}

static PyModuleDef PyXmlSecModule = {
    PyModuleDef_HEAD_INIT,
    STRINGIFY(MODULE_NAME), /* name of module */
    MODULE_DOC,             /* module documentation, may be NULL */
    -1,                     /* size of per-interpreter state of the module,
                               or -1 if the module keeps state in global variables. */
    PyXmlSec_MainMethods,   /* m_methods */
    NULL,                   /* m_slots */
    NULL,                   /* m_traverse */
    PyXmlSec_PyClear,       /* m_clear */
    NULL,                   /* m_free */
};

#define PYENTRY_FUNC_NAME JOIN(PyInit_, MODULE_NAME)
#define PY_MOD_RETURN(m) return m

PyMODINIT_FUNC
PYENTRY_FUNC_NAME(void)
{
    PyObject *module = NULL;
    module = PyModule_Create(&PyXmlSecModule);
    if (!module) {
        PY_MOD_RETURN(NULL); /* this really should never happen */
    }
    PYXMLSEC_DEBUGF("%p", module);

    // init first, since PyXmlSec_Init may raise XmlSecError
    if (PyXmlSec_ExceptionsModule_Init(module) < 0) goto ON_FAIL;

    if (PyXmlSec_Init() < 0) goto ON_FAIL;

    // xmlsec will install default callback in PyXmlSec_Init,
    // overwriting any custom callbacks.
    // We thus install our callback again now.
    PyXmlSec_InstallErrorCallback();

    if (PyModule_AddStringConstant(module, "__version__", STRINGIFY(MODULE_VERSION)) < 0) goto ON_FAIL;

    if (PyXmlSec_InitLxmlModule() < 0) goto ON_FAIL;
    /* Populate final object settings */
    if (PyXmlSec_ConstantsModule_Init(module) < 0) goto ON_FAIL;
    if (PyXmlSec_KeyModule_Init(module) < 0) goto ON_FAIL;
    if (PyXmlSec_TreeModule_Init(module) < 0) goto ON_FAIL;
    if (PyXmlSec_DSModule_Init(module) < 0) goto ON_FAIL;
    if (PyXmlSec_EncModule_Init(module) < 0) goto ON_FAIL;
    if (PyXmlSec_TemplateModule_Init(module) < 0) goto ON_FAIL;

    PY_MOD_RETURN(module);
ON_FAIL:
    PY_MOD_RETURN(NULL);
}
