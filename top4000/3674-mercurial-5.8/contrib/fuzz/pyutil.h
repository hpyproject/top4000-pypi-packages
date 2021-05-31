#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#define PYCODETYPE PyObject
#else
#define PYCODETYPE PyCodeObject
#endif

namespace contrib
{

void initpy(const char *cselfpath);
PyObject *pyglobals();

} /* namespace contrib */
