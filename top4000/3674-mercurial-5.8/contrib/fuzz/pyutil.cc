#include "pyutil.h"

#include <iostream>
#include <string>

namespace contrib
{

#if PY_MAJOR_VERSION >= 3
#define HG_FUZZER_PY3 1
PyMODINIT_FUNC PyInit_parsers(void);
#else
PyMODINIT_FUNC initparsers(void);
#endif

static char cpypath[8192] = "\0";

static PyObject *mainmod;
static PyObject *globals;

void initpy(const char *cselfpath)
{
#ifdef HG_FUZZER_PY3
	const std::string subdir = "/sanpy/lib/python3.8";
#else
	const std::string subdir = "/sanpy/lib/python2.7";
#endif

	/* HACK ALERT: we need a full Python installation built without
	   pymalloc and with ASAN, so we dump one in
	   $OUT/sanpy/lib/python2.7. This helps us wire that up. */
	std::string selfpath(cselfpath);
	std::string pypath;
	auto pos = selfpath.rfind("/");
	if (pos == std::string::npos) {
		char wd[8192];
		if (!getcwd(wd, 8192)) {
			std::cerr << "Failed to call getcwd: errno " << errno
			          << std::endl;
			exit(1);
		}
		pypath = std::string(wd) + subdir;
	} else {
		pypath = selfpath.substr(0, pos) + subdir;
	}
	strncpy(cpypath, pypath.c_str(), pypath.size());
	setenv("PYTHONPATH", cpypath, 1);
	setenv("PYTHONNOUSERSITE", "1", 1);
	/* prevent Python from looking up users in the fuzz environment */
	setenv("PYTHONUSERBASE", cpypath, 1);
#ifdef HG_FUZZER_PY3
	std::wstring wcpypath(pypath.begin(), pypath.end());
	Py_SetPythonHome(wcpypath.c_str());
#else
	Py_SetPythonHome(cpypath);
#endif
	Py_InitializeEx(0);
	mainmod = PyImport_AddModule("__main__");
	globals = PyModule_GetDict(mainmod);

#ifdef HG_FUZZER_PY3
	PyObject *mod = PyInit_parsers();
#else
	initparsers();
	PyObject *mod = PyImport_ImportModule("parsers");
#endif

	PyDict_SetItemString(globals, "parsers", mod);
}

PyObject *pyglobals()
{
	return globals;
}

} // namespace contrib
