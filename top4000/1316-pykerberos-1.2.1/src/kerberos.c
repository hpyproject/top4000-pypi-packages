/**
 * Copyright (c) 2006-2013 Apple Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#include <Python.h>

#include "kerberosbasic.h"
#include "kerberospw.h"
#include "kerberosgss.h"

#if PY_MAJOR_VERSION >= 3
    #define PyInt_FromLong PyLong_FromLong

    #define PyNew(object, destr) PyCapsule_New(object, NULL, destr)
    #define PyCheck(object) PyCapsule_CheckExact(object)
    #define PyGet(object, type) PyCapsule_GetPointer(object, NULL)
    #define PyClear(object) PyCapsule_SetPointer(object, NULL)
#else
    #define PyNew(object, destr) PyCObject_FromVoidPtr(object, destr)
    #define PyCheck(object) PyCObject_Check(object)
    #define PyGet(object, type) (type *)PyCObject_AsVoidPtr(object)
    #define PyClear(object) PyCObject_SetVoidPtr(object, NULL)
#endif

static char krb5_mech_oid_bytes [] = "\x2a\x86\x48\x86\xf7\x12\x01\x02\x02";
gss_OID_desc krb5_mech_oid = { 9, &krb5_mech_oid_bytes };

static char spnego_mech_oid_bytes[] = "\x2b\x06\x01\x05\x05\x02";
gss_OID_desc spnego_mech_oid = { 6, &spnego_mech_oid_bytes };

PyObject *KrbException_class;
PyObject *BasicAuthException_class;
PyObject *PwdChangeException_class;
PyObject *GssException_class;

static PyObject *checkPassword(PyObject *self, PyObject *args) {
    const char *user = NULL;
    const char *pswd = NULL;
    const char *service = NULL;
    const char *default_realm = NULL;
    int verify = 1;
    int result = 0;

    if (!PyArg_ParseTuple(args, "ssss|b", &user, &pswd, &service, &default_realm, &verify)) {
        return NULL;
    }

    result = authenticate_user_krb5pwd(user, pswd, service, default_realm, verify);

    if (result) {
        return Py_INCREF(Py_True), Py_True;
    } else {
        return NULL;
    }
}

static PyObject *changePassword(PyObject *self, PyObject *args) {
    const char *newpswd = NULL;
    const char *oldpswd = NULL;
    const char *user = NULL;
    int result = 0;

    if (!PyArg_ParseTuple(args, "sss", &user, &oldpswd, &newpswd)) {
        return NULL;
    }

    result = change_user_krb5pwd(user, oldpswd, newpswd);

    if (result) {
        return Py_INCREF(Py_True), Py_True;
    } else {
        return NULL;
    }
}

static PyObject *getServerPrincipalDetails(PyObject *self, PyObject *args) {
    const char *service = NULL;
    const char *hostname = NULL;
    char* result;

    if (!PyArg_ParseTuple(args, "ss", &service, &hostname)) {
        return NULL;
    }

    result = server_principal_details(service, hostname);

    if (result != NULL) {
        PyObject* pyresult = Py_BuildValue("s", result);
        free(result);
        return pyresult;
    } else {
        return NULL;
    }
}

#if PY_MAJOR_VERSION >= 3
void destruct_client(PyObject* o) {
    gss_client_state *state;
    state = PyCapsule_GetPointer(o, NULL);
#else
void destruct_client(void* o) {
    gss_client_state *state;
    state = (gss_client_state *)o;
#endif

    if (state != NULL) {
        authenticate_gss_client_clean(state);
        free(state);
    }
}

static PyObject* authGSSClientInit(PyObject* self, PyObject* args, PyObject* keywds) {
    const char *service = NULL;
    const char *principal = NULL;
    gss_client_state *state;
    PyObject *pystate;
    gss_OID mech_oid = GSS_C_NO_OID;
    PyObject *pymech_oid = NULL;
    static char *kwlist[] = {"service", "principal", "gssflags", "mech_oid", NULL};
    long int gss_flags = GSS_C_MUTUAL_FLAG | GSS_C_SEQUENCE_FLAG;
    int result = 0;

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|zlO", kwlist, &service, &principal, &gss_flags, &pymech_oid)) {
        return NULL;
    }

    if (pymech_oid != NULL) {
        if (!PyCheck(pymech_oid)) {
            PyErr_SetString(PyExc_TypeError, "Invalid type for mech_oid");
            return NULL;
        }
        mech_oid = PyGet(pymech_oid, gss_OID_desc);
        if (mech_oid == NULL) {
            PyErr_SetString(PyExc_TypeError, "Invalid value for mech_oid");
            return NULL;
        }
    }

    state = (gss_client_state *) malloc(sizeof(gss_client_state));
    pystate = PyNew(state, &destruct_client);

    result = authenticate_gss_client_init(service, principal, gss_flags, mech_oid, state);
    if (result == AUTH_GSS_ERROR) {
        return NULL;
    }

    return Py_BuildValue("(iN)", result, pystate);
}

static PyObject *authGSSClientClean(PyObject *self, PyObject *args) {
    return Py_BuildValue("i", AUTH_GSS_COMPLETE);
}

#if PY_MAJOR_VERSION >= 3
void destruct_channel_bindings(PyObject* o) {
    struct gss_channel_bindings_struct *channel_bindings = PyCapsule_GetPointer(o, NULL);
#else
void destruct_channel_bindings(void* o) {
    struct gss_channel_bindings_struct *channel_bindings = (struct gss_channel_bindings_struct *)o;
#endif

    if (channel_bindings != NULL) {
        if (channel_bindings->initiator_address.value != NULL) {
            PyMem_Free(channel_bindings->initiator_address.value);
        }

        if (channel_bindings->acceptor_address.value != NULL) {
            PyMem_Free(channel_bindings->acceptor_address.value);
        }

        if (channel_bindings->application_data.value != NULL) {
            PyMem_Free(channel_bindings->application_data.value);
        }

        free(channel_bindings);
    }
}

static PyObject *channelBindings(PyObject *self, PyObject *args, PyObject* keywds) {
    int initiator_addrtype = GSS_C_AF_UNSPEC;
    int acceptor_addrtype = GSS_C_AF_UNSPEC;

    const char *encoding = NULL;
    char *initiator_address = NULL;
    char *acceptor_address = NULL;
    char *application_data = NULL;
    int initiator_length = 0;
    int acceptor_length = 0;
    int application_length = 0;

    PyObject *pychan_bindings = NULL;
    struct gss_channel_bindings_struct *input_chan_bindings;
    static char *kwlist[] = {"initiator_addrtype", "initiator_address", "acceptor_addrtype",
        "acceptor_address", "application_data", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "|iet#iet#et#", kwlist,
            &initiator_addrtype, &encoding, &initiator_address, &initiator_length,
            &acceptor_addrtype, &encoding, &acceptor_address, &acceptor_length,
            &encoding, &application_data, &application_length)) {
        return NULL;
    }

    input_chan_bindings = (struct gss_channel_bindings_struct *) malloc(sizeof(struct gss_channel_bindings_struct));
    pychan_bindings = PyNew(input_chan_bindings, &destruct_channel_bindings);

    input_chan_bindings->initiator_addrtype = initiator_addrtype;
    input_chan_bindings->initiator_address.length = initiator_length;
    input_chan_bindings->initiator_address.value = initiator_address;

    input_chan_bindings->acceptor_addrtype = acceptor_addrtype;
    input_chan_bindings->acceptor_address.length = acceptor_length;
    input_chan_bindings->acceptor_address.value = acceptor_address;

    input_chan_bindings->application_data.length = application_length;
    input_chan_bindings->application_data.value = application_data;

    return Py_BuildValue("N", pychan_bindings);
}

static PyObject *authGSSClientStep(PyObject *self, PyObject *args, PyObject* keywds) {
    gss_client_state *state;
    PyObject *pystate;
    char *challenge = NULL;
    PyObject *pychan_bindings = NULL;
    struct gss_channel_bindings_struct *channel_bindings;
    static char *kwlist[] = {"state", "challenge", "channel_bindings", NULL};
    int result = 0;

    if (! PyArg_ParseTupleAndKeywords(args, keywds, "Os|O", kwlist, &pystate, &challenge, &pychan_bindings)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        return NULL;
    }

    if (pychan_bindings == NULL) {
        channel_bindings = GSS_C_NO_CHANNEL_BINDINGS;
    } else {
        if (!PyCheck(pychan_bindings)) {
            PyErr_SetString(PyExc_TypeError, "Expected a gss_channel_bindings_struct object");
            return NULL;
        }
        channel_bindings = PyGet(pychan_bindings, struct gss_channel_bindings_struct);
    }

    result = authenticate_gss_client_step(state, challenge, channel_bindings);
    if (result == AUTH_GSS_ERROR) {
        return NULL;
    }

    return Py_BuildValue("i", result);
}

static PyObject *authGSSClientResponseConf(PyObject *self, PyObject *args) {
    gss_client_state *state;
    PyObject *pystate;

    if (!PyArg_ParseTuple(args, "O", &pystate)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        return NULL;
    }

    return Py_BuildValue("i", state->responseConf);
}

static PyObject *authGSSClientResponse(PyObject *self, PyObject *args) {
    gss_client_state *state;
    PyObject *pystate;

    if (!PyArg_ParseTuple(args, "O", &pystate)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        return NULL;
    }

    return Py_BuildValue("s", state->response);
}

static PyObject *authGSSClientUserName(PyObject *self, PyObject *args) {
    gss_client_state *state;
    PyObject *pystate;

    if (!PyArg_ParseTuple(args, "O", &pystate)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        return NULL;
    }

    return Py_BuildValue("s", state->username);
}

static PyObject *authGSSClientUnwrap(PyObject *self, PyObject *args) {
    gss_client_state *state;
    PyObject *pystate;
	  char *challenge = NULL;
	  int result = 0;

    if (!PyArg_ParseTuple(args, "Os", &pystate, &challenge)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        return NULL;
    }

    result = authenticate_gss_client_unwrap(state, challenge);
    if (result == AUTH_GSS_ERROR) {
        return NULL;
    }

    return Py_BuildValue("i", result);
}

#ifdef GSSAPI_EXT
static PyObject *authGSSClientUnwrapIov(PyObject *self, PyObject *args) {
    gss_client_state *state;
    PyObject *pystate;
    char *challenge = NULL;
    int result = 0;

    if (!PyArg_ParseTuple(args, "Os", &pystate, &challenge)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        return NULL;
    }

    result = authenticate_gss_client_unwrap_iov(state, challenge);
    if (result == AUTH_GSS_ERROR) {
        return NULL;
    }

    return Py_BuildValue("i", result);
}
#endif

static PyObject *authGSSClientWrap(PyObject *self, PyObject *args) {
    gss_client_state *state;
    PyObject *pystate;
    char *challenge = NULL;
    char *user = NULL;
    int protect = 0;
    int result = 0;

    if (!PyArg_ParseTuple(args, "Os|zi", &pystate, &challenge, &user, &protect)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        return NULL;
    }

    result = authenticate_gss_client_wrap(state, challenge, user, protect);
    if (result == AUTH_GSS_ERROR) {
        return NULL;
    }

    return Py_BuildValue("i", result);
}

#ifdef GSSAPI_EXT
static PyObject *authGSSClientWrapIov(PyObject *self, PyObject *args) {
    gss_client_state *state;
    PyObject *pystate;
    char *challenge = NULL;
    int protect = 1;
    int result = 0;
    int pad_len = 0;

    if (!PyArg_ParseTuple(args, "Os|i", &pystate, &challenge, &protect)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        return NULL;
    }

    result = authenticate_gss_client_wrap_iov(state, challenge, protect, &pad_len);
    if (result == AUTH_GSS_ERROR) {
        return NULL;
    }

    return Py_BuildValue("ii", result, pad_len);
}
#endif

#if PY_MAJOR_VERSION >= 3
void destruct_server(PyObject* o) {
    gss_server_state *state;
    state = PyCapsule_GetPointer(o, NULL);
#else
void destruct_server(void* o) {
    gss_server_state *state;
    state = (gss_server_state *)o;
#endif

    if (state != NULL) {
        authenticate_gss_server_clean(state);
        free(state);
    }
}

static PyObject *authGSSServerInit(PyObject *self, PyObject *args) {
    const char *service = NULL;
    gss_server_state *state;
    PyObject *pystate;
    int result = 0;

    if (!PyArg_ParseTuple(args, "s", &service)) {
        return NULL;
    }

    state = (gss_server_state *) malloc(sizeof(gss_server_state));
    pystate = PyNew(state, &destruct_server);

    result = authenticate_gss_server_init(service, state);
    if (result == AUTH_GSS_ERROR) {
        return NULL;
    }

    return Py_BuildValue("(iN)", result, pystate);
}

static PyObject *authGSSServerClean(PyObject *self, PyObject *args) {
    return Py_BuildValue("i", AUTH_GSS_COMPLETE);
}

static PyObject *authGSSServerStep(PyObject *self, PyObject *args) {
    gss_server_state *state;
    PyObject *pystate;
    char *challenge = NULL;
    int result = 0;

    if (!PyArg_ParseTuple(args, "Os", &pystate, &challenge)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_server_state);
    if (state == NULL) {
        return NULL;
    }

    result = authenticate_gss_server_step(state, challenge);
    if (result == AUTH_GSS_ERROR) {
        return NULL;
    }

    return Py_BuildValue("i", result);
}

static PyObject *authGSSServerResponse(PyObject *self, PyObject *args) {
    gss_server_state *state;
    PyObject *pystate;

    if (!PyArg_ParseTuple(args, "O", &pystate)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_server_state);
    if (state == NULL) {
        return NULL;
    }

    return Py_BuildValue("s", state->response);
}

static PyObject *authGSSServerUserName(PyObject *self, PyObject *args) {
    gss_server_state *state;
    PyObject *pystate;

    if (!PyArg_ParseTuple(args, "O", &pystate)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_server_state);
    if (state == NULL) {
        return NULL;
    }

    return Py_BuildValue("s", state->username);
}

static PyObject *authGSSServerTargetName(PyObject *self, PyObject *args) {
    gss_server_state *state;
    PyObject *pystate;

    if (!PyArg_ParseTuple(args, "O", &pystate)) {
        return NULL;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        return NULL;
    }

    state = PyGet(pystate, gss_server_state);
    if (state == NULL) {
        return NULL;
    }

    return Py_BuildValue("s", state->targetname);
}

#ifdef GSSAPI_EXT
static PyObject* authGSSWinRMEncryptMessage(PyObject* self, PyObject* args)
{
    char *input = NULL;
    char *header = NULL;
    int header_len = 0;
    char *enc_output = NULL;
    int enc_output_len = 0;
    PyObject *pystate = NULL;
    gss_client_state *state = NULL;
    int result = 0;
    PyObject *pyresult = NULL;

    // NB: use et so we get a copy of the string (since gss_wrap_iov mutates it), and so we're certain it's always
    // a UTF8 byte string
    if (! PyArg_ParseTuple(args, "Oet", &pystate, "UTF-8", &input)) {
        pyresult = NULL;
        goto end;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        pyresult = NULL;
        goto end;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        pyresult = NULL;
        goto end;
    }

    result = encrypt_message(state, input, &header, &header_len, &enc_output, &enc_output_len);

    if (result == AUTH_GSS_ERROR) {
        pyresult = NULL;
        goto end;
    }

#if PY_MAJOR_VERSION >= 3
    pyresult = Py_BuildValue("y# y#", enc_output, enc_output_len, header, header_len);
#else
    pyresult = Py_BuildValue("s# s#", enc_output, enc_output_len, header, header_len);
#endif

end:
    if (input) {
        PyMem_Free(input);
    }
    if (header) {
        free(header);
    }
    if (enc_output) {
        free(enc_output);
    }

    return pyresult;
}

static PyObject* authGSSWinRMDecryptMessage(PyObject* self, PyObject* args)
{
    char *header = NULL;
    int header_len = 0;
    char *enc_data = NULL;
    int enc_data_len = 0;
    PyObject *pystate = NULL;
    PyObject *pyheader = NULL;
    PyObject *pyenc_data = NULL;
    gss_client_state *state = NULL;
    char *dec_output = NULL;
    int dec_output_len = 0;
    int result = 0;
    PyObject *pyresult = 0;

    // NB: since the sig/data strings are arbitrary binary and don't conform to
    // a valid encoding, none of the normal string marshaling types will work. We'll
    // have to extract the data later.
    if (! PyArg_ParseTuple(args, "OOO", &pystate, &pyenc_data, &pyheader)) {
        pyresult = NULL;
        goto end;
    }

    if (!PyCheck(pystate)) {
        PyErr_SetString(PyExc_TypeError, "Expected a context object");
        pyresult = NULL;
        goto end;
    }

    state = PyGet(pystate, gss_client_state);
    if (state == NULL) {
        pyresult = NULL;
        goto end;
    }

    // request the length and copy the header and encrypted input data from the Python strings
    header_len = (int) PyBytes_Size(pyheader);
    header = malloc(header_len);
    memcpy(header, PyBytes_AsString(pyheader), header_len);

    enc_data_len = (int) PyBytes_Size(pyenc_data);
    enc_data = malloc(enc_data_len);
    memcpy(enc_data, PyBytes_AsString(pyenc_data), enc_data_len);

    result = decrypt_message(state, header, header_len, enc_data, enc_data_len, &dec_output, &dec_output_len);

    if (result == AUTH_GSS_ERROR) {
        pyresult = NULL;
        goto end;
    }

#if PY_MAJOR_VERSION >= 3
    pyresult = Py_BuildValue("y#", dec_output, dec_output_len);
#else
    pyresult = Py_BuildValue("s#", dec_output, dec_output_len);
#endif

end:
    if (header) {
        free(header);
    }
    if (enc_data) {
        free(enc_data);
    }
    if (dec_output) {
        free(dec_output);
    }

    return pyresult;
}
#endif

static PyMethodDef KerberosMethods[] = {
    {"checkPassword",  checkPassword, METH_VARARGS,
     "Check the supplied user/password against Kerberos KDC."},
    {"changePassword",  changePassword, METH_VARARGS,
     "Change the user password."},
    {"getServerPrincipalDetails",  getServerPrincipalDetails, METH_VARARGS,
     "Return the service principal for a given service and hostname."},
    {"authGSSClientInit",  (PyCFunction)authGSSClientInit, METH_VARARGS | METH_KEYWORDS,
     "Initialize client-side GSSAPI operations."},
    {"authGSSClientClean",  authGSSClientClean, METH_VARARGS,
     "Terminate client-side GSSAPI operations."},
    {"authGSSClientStep",  (PyCFunction)authGSSClientStep, METH_VARARGS | METH_KEYWORDS,
     "Do a client-side GSSAPI step."},
    {"authGSSClientResponse",  authGSSClientResponse, METH_VARARGS,
     "Get the response from the last client-side GSSAPI step."},
    {"authGSSClientResponseConf",  authGSSClientResponseConf, METH_VARARGS,
     "return 1 if confidentiality was set in the last unwrapped buffer, 0 otherwise."},
    {"authGSSClientUserName",  authGSSClientUserName, METH_VARARGS,
     "Get the user name from the last client-side GSSAPI step."},
    {"authGSSServerInit",  authGSSServerInit, METH_VARARGS,
     "Initialize server-side GSSAPI operations."},
    {"authGSSClientWrap",  authGSSClientWrap, METH_VARARGS,
     "Do a GSSAPI wrap."},
    {"authGSSClientUnwrap",  authGSSClientUnwrap, METH_VARARGS,
     "Do a GSSAPI unwrap."},
    {"channelBindings",  (PyCFunction)channelBindings, METH_VARARGS | METH_KEYWORDS,
     "Build the Channel Bindings Structure based on the input."},
#ifdef GSSAPI_EXT
    {"authGSSClientWrapIov",  authGSSClientWrapIov, METH_VARARGS,
     "Do a GSSAPI iov wrap."},
    {"authGSSClientUnwrapIov",  authGSSClientUnwrapIov, METH_VARARGS,
     "Do a GSSAPI iov unwrap."},
    {"authGSSWinRMEncryptMessage",  authGSSWinRMEncryptMessage, METH_VARARGS,
     "Encrypt a message"},
    {"authGSSWinRMDecryptMessage",  authGSSWinRMDecryptMessage, METH_VARARGS,
     "Decrypt a message"},
#endif
    {"authGSSServerClean",  authGSSServerClean, METH_VARARGS,
     "Terminate server-side GSSAPI operations."},
    {"authGSSServerStep",  authGSSServerStep, METH_VARARGS,
     "Do a server-side GSSAPI step."},
    {"authGSSServerResponse",  authGSSServerResponse, METH_VARARGS,
     "Get the response from the last server-side GSSAPI step."},
    {"authGSSServerUserName",  authGSSServerUserName, METH_VARARGS,
        "Get the user name from the last server-side GSSAPI step."},
    {"authGSSServerTargetName",  authGSSServerTargetName, METH_VARARGS,
        "Get the target name from the last server-side GSSAPI step."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
    static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "kerberos",          /* m_name */
        "High-level interface to kerberos",  /* m_doc */
        -1,                  /* m_size */
        KerberosMethods,     /* m_methods */
        NULL,                /* m_reload */
        NULL,                /* m_traverse */
        NULL,                /* m_clear */
        NULL,                /* m_free */
    };

#endif



#if PY_MAJOR_VERSION >= 3
PyObject* PyInit_kerberos(void)
#else
void initkerberos(void)
#endif
{
    PyObject *m,*d;

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&moduledef);
#else
    m = Py_InitModule("kerberos", KerberosMethods);
#endif

    d = PyModule_GetDict(m);

    /* create the base exception class */
    if (!(KrbException_class = PyErr_NewException("kerberos.KrbError", NULL, NULL)))
        goto error;
    PyDict_SetItemString(d, "KrbError", KrbException_class);
    Py_INCREF(KrbException_class);

    /* ...and the derived exceptions */
    if (!(BasicAuthException_class = PyErr_NewException("kerberos.BasicAuthError", KrbException_class, NULL)))
        goto error;
    Py_INCREF(BasicAuthException_class);
    PyDict_SetItemString(d, "BasicAuthError", BasicAuthException_class);

    if (!(PwdChangeException_class = PyErr_NewException("kerberos.PwdChangeError", KrbException_class, NULL)))
        goto error;
    Py_INCREF(PwdChangeException_class);
    PyDict_SetItemString(d, "PwdChangeError", PwdChangeException_class);

    if (!(GssException_class = PyErr_NewException("kerberos.GSSError", KrbException_class, NULL)))
        goto error;
    Py_INCREF(GssException_class);
    PyDict_SetItemString(d, "GSSError", GssException_class);

    PyDict_SetItemString(d, "AUTH_GSS_COMPLETE", PyInt_FromLong(AUTH_GSS_COMPLETE));
    PyDict_SetItemString(d, "AUTH_GSS_CONTINUE", PyInt_FromLong(AUTH_GSS_CONTINUE));

    PyDict_SetItemString(d, "GSS_C_DELEG_FLAG", PyInt_FromLong(GSS_C_DELEG_FLAG));
    PyDict_SetItemString(d, "GSS_C_MUTUAL_FLAG", PyInt_FromLong(GSS_C_MUTUAL_FLAG));
    PyDict_SetItemString(d, "GSS_C_REPLAY_FLAG", PyInt_FromLong(GSS_C_REPLAY_FLAG));
    PyDict_SetItemString(d, "GSS_C_SEQUENCE_FLAG", PyInt_FromLong(GSS_C_SEQUENCE_FLAG));
    PyDict_SetItemString(d, "GSS_C_CONF_FLAG", PyInt_FromLong(GSS_C_CONF_FLAG));
    PyDict_SetItemString(d, "GSS_C_INTEG_FLAG", PyInt_FromLong(GSS_C_INTEG_FLAG));
    PyDict_SetItemString(d, "GSS_C_ANON_FLAG", PyInt_FromLong(GSS_C_ANON_FLAG));
    PyDict_SetItemString(d, "GSS_C_PROT_READY_FLAG", PyInt_FromLong(GSS_C_PROT_READY_FLAG));
    PyDict_SetItemString(d, "GSS_C_TRANS_FLAG", PyInt_FromLong(GSS_C_TRANS_FLAG));
    PyDict_SetItemString(d, "GSS_MECH_OID_KRB5", PyNew(&krb5_mech_oid, NULL));
    PyDict_SetItemString(d, "GSS_MECH_OID_SPNEGO", PyNew(&spnego_mech_oid, NULL));

    PyDict_SetItemString(d, "GSS_C_AF_UNSPEC", PyInt_FromLong(GSS_C_AF_UNSPEC));
    PyDict_SetItemString(d, "GSS_C_AF_LOCAL", PyInt_FromLong(GSS_C_AF_LOCAL));
    PyDict_SetItemString(d, "GSS_C_AF_INET", PyInt_FromLong(GSS_C_AF_INET));
    PyDict_SetItemString(d, "GSS_C_AF_IMPLINK", PyInt_FromLong(GSS_C_AF_IMPLINK));
    PyDict_SetItemString(d, "GSS_C_AF_PUP", PyInt_FromLong(GSS_C_AF_PUP));
    PyDict_SetItemString(d, "GSS_C_AF_CHAOS", PyInt_FromLong(GSS_C_AF_CHAOS));
    PyDict_SetItemString(d, "GSS_C_AF_NS", PyInt_FromLong(GSS_C_AF_NS));
    PyDict_SetItemString(d, "GSS_C_AF_NBS", PyInt_FromLong(GSS_C_AF_NBS));
    PyDict_SetItemString(d, "GSS_C_AF_ECMA", PyInt_FromLong(GSS_C_AF_ECMA));
    PyDict_SetItemString(d, "GSS_C_AF_DATAKIT", PyInt_FromLong(GSS_C_AF_DATAKIT));
    PyDict_SetItemString(d, "GSS_C_AF_CCITT", PyInt_FromLong(GSS_C_AF_CCITT));
    PyDict_SetItemString(d, "GSS_C_AF_SNA", PyInt_FromLong(GSS_C_AF_SNA));
    PyDict_SetItemString(d, "GSS_C_AF_DECnet", PyInt_FromLong(GSS_C_AF_DECnet));
    PyDict_SetItemString(d, "GSS_C_AF_DLI", PyInt_FromLong(GSS_C_AF_DLI));
    PyDict_SetItemString(d, "GSS_C_AF_LAT", PyInt_FromLong(GSS_C_AF_LAT));
    PyDict_SetItemString(d, "GSS_C_AF_HYLINK", PyInt_FromLong(GSS_C_AF_HYLINK));
    PyDict_SetItemString(d, "GSS_C_AF_APPLETALK", PyInt_FromLong(GSS_C_AF_APPLETALK));
    PyDict_SetItemString(d, "GSS_C_AF_BSC", PyInt_FromLong(GSS_C_AF_BSC));
    PyDict_SetItemString(d, "GSS_C_AF_DSS", PyInt_FromLong(GSS_C_AF_DSS));
    PyDict_SetItemString(d, "GSS_C_AF_OSI", PyInt_FromLong(GSS_C_AF_OSI));
    PyDict_SetItemString(d, "GSS_C_AF_X25", PyInt_FromLong(GSS_C_AF_X25));
    PyDict_SetItemString(d, "GSS_C_AF_NULLADDR", PyInt_FromLong(GSS_C_AF_NULLADDR));

error:
    if (PyErr_Occurred())
        PyErr_SetString(PyExc_ImportError, "kerberos: init failed");
#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
