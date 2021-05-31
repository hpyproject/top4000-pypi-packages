#include "Types.hpp"

PyObject * MGLContext_renderbuffer(MGLContext * self, PyObject * args) {
	int width;
	int height;

	int components;

	int samples;

	const char * dtype;
	Py_ssize_t dtype_size;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)IIs#",
		&width,
		&height,
		&components,
		&samples,
		&dtype,
		&dtype_size
	);

	if (!args_ok) {
		return 0;
	}

	if (components < 1 || components > 4) {
		MGLError_Set("the components must be 1, 2, 3 or 4");
		return 0;
	}

	if ((samples & (samples - 1)) || samples > self->max_samples) {
		MGLError_Set("the number of samples is invalid");
		return 0;
	}

	if (dtype_size != 2) {
		MGLError_Set("invalid dtype");
		return 0;
	}

	MGLDataType * data_type = from_dtype(dtype);

	if (!data_type) {
		MGLError_Set("invalid dtype");
		return 0;
	}

	int format = data_type->internal_format[components];

	const GLMethods & gl = self->gl;

	MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)MGLRenderbuffer_Type.tp_alloc(&MGLRenderbuffer_Type, 0);

	renderbuffer->renderbuffer_obj = 0;
	gl.GenRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);

	if (!renderbuffer->renderbuffer_obj) {
		MGLError_Set("cannot create renderbuffer");
		Py_DECREF(renderbuffer);
		return 0;
	}

	gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer->renderbuffer_obj);

	if (samples == 0) {
		gl.RenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	} else {
		gl.RenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
	}

	renderbuffer->width = width;
	renderbuffer->height = height;
	renderbuffer->components = components;
	renderbuffer->samples = samples;
	renderbuffer->data_type = data_type;
	renderbuffer->depth = false;

	Py_INCREF(self);
	renderbuffer->context = self;

	Py_INCREF(renderbuffer);

	PyObject * result = PyTuple_New(2);
	PyTuple_SET_ITEM(result, 0, (PyObject *)renderbuffer);
	PyTuple_SET_ITEM(result, 1, PyLong_FromLong(renderbuffer->renderbuffer_obj));
	return result;
}

PyObject * MGLContext_depth_renderbuffer(MGLContext * self, PyObject * args) {
	int width;
	int height;

	int samples;

	int args_ok = PyArg_ParseTuple(
		args,
		"(II)I",
		&width,
		&height,
		&samples
	);

	if (!args_ok) {
		return 0;
	}

	if ((samples & (samples - 1)) || samples > self->max_samples) {
		MGLError_Set("the number of samples is invalid");
		return 0;
	}

	const GLMethods & gl = self->gl;

	MGLRenderbuffer * renderbuffer = (MGLRenderbuffer *)MGLRenderbuffer_Type.tp_alloc(&MGLRenderbuffer_Type, 0);

	renderbuffer->renderbuffer_obj = 0;
	gl.GenRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);

	if (!renderbuffer->renderbuffer_obj) {
		MGLError_Set("cannot create renderbuffer");
		Py_DECREF(renderbuffer);
		return 0;
	}

	gl.BindRenderbuffer(GL_RENDERBUFFER, renderbuffer->renderbuffer_obj);

	if (samples == 0) {
		gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	} else {
		gl.RenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);
	}

	renderbuffer->width = width;
	renderbuffer->height = height;
	renderbuffer->components = 1;
	renderbuffer->samples = samples;
	renderbuffer->data_type = from_dtype("f4");
	renderbuffer->depth = true;

	Py_INCREF(self);
	renderbuffer->context = self;

	Py_INCREF(renderbuffer);

	PyObject * result = PyTuple_New(2);
	PyTuple_SET_ITEM(result, 0, (PyObject *)renderbuffer);
	PyTuple_SET_ITEM(result, 1, PyLong_FromLong(renderbuffer->renderbuffer_obj));
	return result;
}

PyObject * MGLRenderbuffer_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLRenderbuffer * self = (MGLRenderbuffer *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLRenderbuffer_tp_dealloc(MGLRenderbuffer * self) {
	MGLRenderbuffer_Type.tp_free((PyObject *)self);
}

PyObject * MGLRenderbuffer_release(MGLRenderbuffer * self) {
	MGLRenderbuffer_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLRenderbuffer_tp_methods[] = {
	{"release", (PyCFunction)MGLRenderbuffer_release, METH_NOARGS, 0},
	{0},
};

PyTypeObject MGLRenderbuffer_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Renderbuffer",                                     // tp_name
	sizeof(MGLRenderbuffer),                                // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLRenderbuffer_tp_dealloc,                 // tp_dealloc
	0,                                                      // tp_print
	0,                                                      // tp_getattr
	0,                                                      // tp_setattr
	0,                                                      // tp_reserved
	0,                                                      // tp_repr
	0,                                                      // tp_as_number
	0,                                                      // tp_as_sequence
	0,                                                      // tp_as_mapping
	0,                                                      // tp_hash
	0,                                                      // tp_call
	0,                                                      // tp_str
	0,                                                      // tp_getattro
	0,                                                      // tp_setattro
	0,                                                      // tp_as_buffer
	Py_TPFLAGS_DEFAULT,                                     // tp_flags
	0,                                                      // tp_doc
	0,                                                      // tp_traverse
	0,                                                      // tp_clear
	0,                                                      // tp_richcompare
	0,                                                      // tp_weaklistoffset
	0,                                                      // tp_iter
	0,                                                      // tp_iternext
	MGLRenderbuffer_tp_methods,                             // tp_methods
	0,                                                      // tp_members
	0,                                                      // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLRenderbuffer_tp_new,                                 // tp_new
};

void MGLRenderbuffer_Invalidate(MGLRenderbuffer * renderbuffer) {
	if (Py_TYPE(renderbuffer) == &MGLInvalidObject_Type) {
		return;
	}

	// TODO: decref

	const GLMethods & gl = renderbuffer->context->gl;
	gl.DeleteRenderbuffers(1, (GLuint *)&renderbuffer->renderbuffer_obj);

	Py_TYPE(renderbuffer) = &MGLInvalidObject_Type;
	Py_DECREF(renderbuffer);
}
