/*
 * Copyright (c) 2006 Damien Miller <djm@mindrot.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "pybc_blf.h"

#if PY_VERSION_HEX < 0x02050000
typedef int Py_ssize_t;
#endif

#define PYBCRYPT_VERSION "0.4"

#if defined(_WIN32)
#define bzero(s,n) memset(s, '\0', n)
#endif

/* $Id$ */

/* Import */
int pybc_bcrypt(const char *, const char *, char *, size_t);
void encode_salt(char *, u_int8_t *, u_int16_t, u_int8_t);

PyDoc_STRVAR(bcrypt_encode_salt_doc,
"encode_salt(csalt, log_rounds) -> encoded_salt\n\
    Encode a raw binary salt and the specified log2(rounds) as a\n\
    standard bcrypt text salt. Used internally by bcrypt.gensalt()\n");

static PyObject *
bcrypt_encode_salt(PyObject *self, PyObject *args, PyObject *kw_args)
{
	static char *keywords[] = { "csalt", "log_rounds", NULL };
	u_int8_t *csalt = NULL;
	Py_ssize_t csaltlen = -1;
	long log_rounds = -1;
	char ret[64];

	if (!PyArg_ParseTupleAndKeywords(args, kw_args, "s#l:encode_salt",
	    keywords, &csalt, &csaltlen, &log_rounds))
                return NULL;
	if (csaltlen != 16) {
		PyErr_SetString(PyExc_ValueError, "Invalid salt length");
		return NULL;
	}
	if (log_rounds < 4 || log_rounds > 31) {
		PyErr_SetString(PyExc_ValueError, "Invalid number of rounds");
		return NULL;
	}
	encode_salt(ret, csalt, csaltlen, log_rounds);
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_FromString(ret);
#else
	return PyString_FromString(ret);
#endif
}

/* Check that a string has no embedded '\0' characters and duplicate it. */
static char *
checkdup(const char *s, Py_ssize_t len)
{
	Py_ssize_t i;
	char *ret;

	if (len < 0)
		return NULL;
	for (i = 0; i < len; i++) {
		if (s[i] == '\0')
			return NULL;
	}
	if ((ret = malloc(len + 1)) == NULL)
		return NULL;
	memcpy(ret, s, len);
	ret[len] = '\0';
	return ret;
}

PyDoc_STRVAR(bcrypt_hashpw_doc,
"hashpw(password, salt) -> hashed_password\n\
    Hash the specified password and the salt using the OpenBSD\n\
    Blowfish password hashing algorithm. Returns the hashed password.\n");

static PyObject *
bcrypt_hashpw(PyObject *self, PyObject *args, PyObject *kw_args)
{
	static char *keywords[] = { "password", "salt", NULL };
	char *password = NULL, *salt = NULL;
	char hashed[128], *password_copy, *salt_copy;
	Py_ssize_t password_len = -1, salt_len = -1;
	int ret;

	if (!PyArg_ParseTupleAndKeywords(args, kw_args, "s#s#:hashpw", keywords,
	    &password, &password_len, &salt, &salt_len))
                return NULL;

	if (password_len < 0 || password_len > 65535) {
		PyErr_SetString(PyExc_ValueError,
		    "unsupported password length");
		return NULL;
	}
	if (salt_len < 0 || salt_len > 65535) {
		PyErr_SetString(PyExc_ValueError, "unsupported salt length");
		return NULL;
	}
	if ((password_copy = checkdup(password, password_len)) == NULL) {
		PyErr_SetString(PyExc_ValueError,
		    "password must not contain nul characters");
		return NULL;
	}
	password_len = 0;
	if ((salt_copy = checkdup(salt, salt_len)) == NULL) {
		PyErr_SetString(PyExc_ValueError,
		    "salt must not contain nul characters");
		bzero(password_copy, strlen(password_copy));
		free(password_copy);
		return NULL;
	}
	Py_BEGIN_ALLOW_THREADS;
	ret = pybc_bcrypt(password_copy, salt_copy, hashed, sizeof(hashed));
	Py_END_ALLOW_THREADS;

	bzero(password_copy, strlen(password_copy));
	free(password_copy);
	bzero(salt_copy, strlen(salt_copy));
	free(salt_copy);
	if (ret != 0 || strlen(hashed) < 32) {
		PyErr_SetString(PyExc_ValueError, "Invalid salt");
		return NULL;
	}
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_FromString(hashed);
#else
	return PyString_FromString(hashed);
#endif
}

PyDoc_STRVAR(bcrypt_checkpw_doc,
"checkpw(password, hashed_password) -> boolean\n\
    Verify that the plaintext password matches hashed_password. Returns true\n\
    if it matches or false otherwise.\n");

static PyObject *
bcrypt_checkpw(PyObject *self, PyObject *args, PyObject *kw_args)
{
	static char *keywords[] = { "password", "hashed_password", NULL };
	char *password = NULL, *expected = NULL;
	char hashed[128], *password_copy, *expected_copy;
	Py_ssize_t password_len = -1, expected_len = -1, hashed_len;
	int ret;

	if (!PyArg_ParseTupleAndKeywords(args, kw_args, "s#s#:checkpw",
	    keywords,
	    &password, &password_len, &expected, &expected_len))
                return NULL;

	if (password_len < 0 || password_len > 65535) {
		PyErr_SetString(PyExc_ValueError,
		    "unsupported password length");
		return NULL;
	}
	if (expected_len < 0 || expected_len > 65535) {
		PyErr_SetString(PyExc_ValueError,
		    "unsupported hashed_password length");
		return NULL;
	}
	if ((password_copy = checkdup(password, password_len)) == NULL) {
		PyErr_SetString(PyExc_ValueError,
		    "password must not contain nul characters");
		return NULL;
	}
	password_len = 0;
	if ((expected_copy = checkdup(expected, expected_len)) == NULL) {
		PyErr_SetString(PyExc_ValueError,
		    "hashed_password must not contain nul characters");
		bzero(password_copy, strlen(password_copy));
		free(password_copy);
		return NULL;
	}

	Py_BEGIN_ALLOW_THREADS;
	ret = pybc_bcrypt(password_copy, expected_copy, hashed, sizeof(hashed));
	Py_END_ALLOW_THREADS;

	bzero(password_copy, strlen(password_copy));
	free(password_copy);
	hashed_len = strlen(hashed);
	if (ret != 0 || hashed_len < 32) {
		PyErr_SetString(PyExc_ValueError,
		    "Invalid hashed_password salt");
		bzero(expected_copy, strlen(expected_copy));
		free(expected_copy);
		return NULL;
	}
	ret = 1; /* fail unless timingsafe_bcmp runs and succeeds */
	if (hashed_len == strlen(expected_copy))
		ret = pybc_timingsafe_bcmp(expected_copy, hashed, hashed_len);
	bzero(hashed, sizeof(hashed));
	bzero(expected_copy, strlen(expected_copy));
	free(expected_copy);
	if (ret == 0)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

PyDoc_STRVAR(bcrypt_kdf_doc,
"kdf(password, salt, desired_key_bytes, rounds) -> key\n\
    Derive \"desired_key_bytes\" (up to 512) cryptographic key material from\n\
    a password and salt. NB. rounds is linear (not an exponent like\n\
    gensalt). Recommended minimum salt length is 16 bytes\n");

#if 0
static void
xdump(const u_int8_t *s, size_t l)
{
	size_t i;

	for (i = 0; i < l; i++)
		printf("\\x%02x", s[i]);
}
#endif

static PyObject *
bcrypt_kdf(PyObject *self, PyObject *args, PyObject *kw_args)
{
	static char *keywords[] = {
		"password",
		"salt",
		"desired_key_bytes",
		"rounds",
		NULL
	};
	const u_int8_t *password = NULL, *salt = NULL;
	Py_ssize_t password_len = -1, salt_len = -1;
	long desired_key_bytes = -1, rounds = -1;
	u_int8_t *key;
	int ret;
	PyObject *o = NULL;

	if (!PyArg_ParseTupleAndKeywords(args, kw_args, "s#s#ll:kdf", keywords,
	    &password, &password_len, &salt, &salt_len,
	    &desired_key_bytes, &rounds))
                return NULL;
	if (password_len <= 0) {
		PyErr_SetString(PyExc_ValueError, "Invalid password length");
		return NULL;
	}
	if (salt_len <= 0) {
		PyErr_SetString(PyExc_ValueError, "Invalid salt length");
		return NULL;
	}
	if (desired_key_bytes <= 0 || desired_key_bytes > 512) {
		PyErr_SetString(PyExc_ValueError, "Invalid output length");
		return NULL;
	}
	if (rounds < 1) {
		PyErr_SetString(PyExc_ValueError, "Invalid number of rounds");
		return NULL;
	}
	if ((key = malloc(desired_key_bytes)) == NULL)
		return NULL;
	Py_BEGIN_ALLOW_THREADS;
	ret = bcrypt_pbkdf(password, password_len, salt, salt_len,
	    key, desired_key_bytes, rounds);
	Py_END_ALLOW_THREADS;
	if (ret != 0)
		PyErr_SetString(PyExc_ValueError, "kdf failed");
	else
		o = PyBytes_FromStringAndSize((char *)key, desired_key_bytes);
	bzero(key, desired_key_bytes);
	free(key);
	return o;
}

static PyMethodDef bcrypt_methods[] = {
	{	"checkpw",	(PyCFunction)bcrypt_checkpw,
		METH_VARARGS|METH_KEYWORDS,	bcrypt_checkpw_doc	},
	{	"hashpw",	(PyCFunction)bcrypt_hashpw,
		METH_VARARGS|METH_KEYWORDS,	bcrypt_hashpw_doc	},
	{	"encode_salt",	(PyCFunction)bcrypt_encode_salt,
		METH_VARARGS|METH_KEYWORDS,	bcrypt_encode_salt_doc	},
	{	"kdf",		(PyCFunction)bcrypt_kdf,
		METH_VARARGS|METH_KEYWORDS,	bcrypt_kdf_doc	},
	{NULL,		NULL}		/* sentinel */
};

PyDoc_STRVAR(module_doc, "Internal module used by bcrypt.\n");

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef bcrypt_module = {
	PyModuleDef_HEAD_INIT,
	"bcrypt._bcrypt",	/* m_name */
	module_doc,		/* m_doc */
	-1,			/* m_size */
	bcrypt_methods,		/* m_methods */
	NULL,			/* m_reload */
	NULL,			/* m_traverse */
	NULL,			/* m_clear */
	NULL,			/* m_free */
};

PyMODINIT_FUNC
PyInit__bcrypt(void)
{
	PyObject *m;

	m = PyModule_Create(&bcrypt_module);
	PyModule_AddStringConstant(m, "__version__", PYBCRYPT_VERSION);
	return m;
}
#else
PyMODINIT_FUNC
init_bcrypt(void)
{
	PyObject *m;

	m = Py_InitModule3("bcrypt._bcrypt", bcrypt_methods, module_doc);
	PyModule_AddStringConstant(m, "__version__", PYBCRYPT_VERSION);
}
#endif
