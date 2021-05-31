#include "Types.hpp"

#include "InlineMethods.hpp"

PyObject * MGLContext_query(MGLContext * self, PyObject * args) {
	int samples_passed;
	int any_samples_passed;
	int time_elapsed;
	int primitives_generated;

	int args_ok = PyArg_ParseTuple(
		args,
		"pppp",
		&samples_passed,
		&any_samples_passed,
		&time_elapsed,
		&primitives_generated
	);

	if (!args_ok) {
		return 0;
	}

	// If none of them is set, all will be set.
	if (!(samples_passed + any_samples_passed + time_elapsed + primitives_generated)) {
		samples_passed = 1;
		any_samples_passed = 1;
		time_elapsed = 1;
		primitives_generated = 1;
	}

	MGLQuery * query = (MGLQuery *)MGLQuery_Type.tp_alloc(&MGLQuery_Type, 0);

	Py_INCREF(self);
	query->context = self;

	const GLMethods & gl = self->gl;

	if (samples_passed) {
		gl.GenQueries(1, (GLuint *)&query->query_obj[SAMPLES_PASSED]);
	}
	if (any_samples_passed) {
		gl.GenQueries(1, (GLuint *)&query->query_obj[ANY_SAMPLES_PASSED]);
	}
	if (time_elapsed) {
		gl.GenQueries(1, (GLuint *)&query->query_obj[TIME_ELAPSED]);
	}
	if (primitives_generated) {
		gl.GenQueries(1, (GLuint *)&query->query_obj[PRIMITIVES_GENERATED]);
	}

	// PyObject * result = PyTuple_New(2);
	// PyTuple_SET_ITEM(result, 0, (PyObject *)query);
	// PyTuple_SET_ITEM(result, 1, PyLong_FromLong(query->query_obj));
	// return result;

	return (PyObject *)query;
}

PyObject * MGLQuery_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLQuery * self = (MGLQuery *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLQuery_tp_dealloc(MGLQuery * self) {
	MGLQuery_Type.tp_free((PyObject *)self);
}

PyObject * MGLQuery_begin(MGLQuery * self, PyObject * args) {
	int args_ok = PyArg_ParseTuple(
		args,
		""
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;

	if (self->query_obj[SAMPLES_PASSED]) {
		gl.BeginQuery(GL_SAMPLES_PASSED, self->query_obj[SAMPLES_PASSED]);
	}

	if (self->query_obj[ANY_SAMPLES_PASSED]) {
		gl.BeginQuery(GL_ANY_SAMPLES_PASSED, self->query_obj[ANY_SAMPLES_PASSED]);
	}

	if (self->query_obj[TIME_ELAPSED]) {
		gl.BeginQuery(GL_TIME_ELAPSED, self->query_obj[TIME_ELAPSED]);
	}

	if (self->query_obj[PRIMITIVES_GENERATED]) {
		gl.BeginQuery(GL_PRIMITIVES_GENERATED, self->query_obj[PRIMITIVES_GENERATED]);
	}

	Py_RETURN_NONE;
}

PyObject * MGLQuery_end(MGLQuery * self, PyObject * args) {
	int args_ok = PyArg_ParseTuple(
		args,
		""
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;

	if (self->query_obj[SAMPLES_PASSED]) {
		gl.EndQuery(GL_SAMPLES_PASSED);
	}

	if (self->query_obj[ANY_SAMPLES_PASSED]) {
		gl.EndQuery(GL_ANY_SAMPLES_PASSED);
	}

	if (self->query_obj[TIME_ELAPSED]) {
		gl.EndQuery(GL_TIME_ELAPSED);
	}

	if (self->query_obj[PRIMITIVES_GENERATED]) {
		gl.EndQuery(GL_PRIMITIVES_GENERATED);
	}

	Py_RETURN_NONE;
}

PyObject * MGLQuery_begin_render(MGLQuery * self, PyObject * args) {
	int args_ok = PyArg_ParseTuple(
		args,
		""
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;

	if (self->query_obj[ANY_SAMPLES_PASSED]) {
		gl.BeginConditionalRender(self->query_obj[ANY_SAMPLES_PASSED], GL_QUERY_NO_WAIT);
	} else if (self->query_obj[SAMPLES_PASSED]) {
		gl.BeginConditionalRender(self->query_obj[SAMPLES_PASSED], GL_QUERY_NO_WAIT);
	} else {
		MGLError_Set("no samples");
		return 0;
	}

	Py_RETURN_NONE;
}

PyObject * MGLQuery_end_render(MGLQuery * self, PyObject * args) {
	int args_ok = PyArg_ParseTuple(
		args,
		""
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;
	gl.EndConditionalRender();
	Py_RETURN_NONE;
}

PyMethodDef MGLQuery_tp_methods[] = {
	{"begin", (PyCFunction)MGLQuery_begin, METH_VARARGS, 0},
	{"end", (PyCFunction)MGLQuery_end, METH_VARARGS, 0},
	{"begin_render", (PyCFunction)MGLQuery_begin_render, METH_VARARGS, 0},
	{"end_render", (PyCFunction)MGLQuery_end_render, METH_VARARGS, 0},
	// {"release", (PyCFunction)MGLQuery_release, METH_NOARGS, 0},
	{0},
};

PyObject * MGLQuery_get_samples(MGLQuery * self) {
	const GLMethods & gl = self->context->gl;

	int samples = 0;
	gl.GetQueryObjectiv(self->query_obj[SAMPLES_PASSED], GL_QUERY_RESULT, &samples);

	return PyLong_FromLong(samples);
}

PyObject * MGLQuery_get_primitives(MGLQuery * self) {
	const GLMethods & gl = self->context->gl;

	int primitives = 0;
	gl.GetQueryObjectiv(self->query_obj[PRIMITIVES_GENERATED], GL_QUERY_RESULT, &primitives);

	return PyLong_FromLong(primitives);
}

PyObject * MGLQuery_get_elapsed(MGLQuery * self) {
	const GLMethods & gl = self->context->gl;

	int elapsed = 0;
	gl.GetQueryObjectiv(self->query_obj[TIME_ELAPSED], GL_QUERY_RESULT, &elapsed);

	return PyLong_FromLong(elapsed);
}

PyGetSetDef MGLQuery_tp_getseters[] = {
	{(char *)"samples", (getter)MGLQuery_get_samples, 0, 0, 0},
	{(char *)"primitives", (getter)MGLQuery_get_primitives, 0, 0, 0},
	{(char *)"elapsed", (getter)MGLQuery_get_elapsed, 0, 0, 0},
	{0},
};

PyTypeObject MGLQuery_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Query",                                            // tp_name
	sizeof(MGLQuery),                                       // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLQuery_tp_dealloc,                        // tp_dealloc
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
	MGLQuery_tp_methods,                                    // tp_methods
	0,                                                      // tp_members
	MGLQuery_tp_getseters,                                  // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLQuery_tp_new,                                        // tp_new
};

void MGLQuery_Invalidate(MGLQuery * query) {
	if (Py_TYPE(query) == &MGLInvalidObject_Type) {
		return;
	}

	// TODO: decref

	// const GLMethods & gl = query->context->gl;

	// TODO: release

	Py_DECREF(query->context);
	Py_TYPE(query) = &MGLInvalidObject_Type;
	Py_DECREF(query);
}
