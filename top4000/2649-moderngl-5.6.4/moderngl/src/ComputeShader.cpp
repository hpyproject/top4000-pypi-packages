#include "Types.hpp"

#include "InlineMethods.hpp"

PyObject * MGLContext_compute_shader(MGLContext * self, PyObject * args) {
	PyObject * source;

	int args_ok = PyArg_ParseTuple(
		args,
		"O",
		&source
	);

	if (!args_ok) {
		return 0;
	}

	if (!PyUnicode_Check(source)) {
		MGLError_Set("the source must be a string not %s", Py_TYPE(source)->tp_name);
		return 0;
	}

	const char * source_str = PyUnicode_AsUTF8(source);

	MGLComputeShader * compute_shader = (MGLComputeShader *)MGLComputeShader_Type.tp_alloc(&MGLComputeShader_Type, 0);

	Py_INCREF(self);
	compute_shader->context = self;

	const GLMethods & gl = self->gl;

	int program_obj = gl.CreateProgram();

	if (!program_obj) {
		MGLError_Set("cannot create program");
		return 0;
	}

	int shader_obj = gl.CreateShader(GL_COMPUTE_SHADER);

	if (!shader_obj) {
		MGLError_Set("cannot create the shader object");
		return 0;
	}

	gl.ShaderSource(shader_obj, 1, &source_str, 0);
	gl.CompileShader(shader_obj);

	int compiled = GL_FALSE;
	gl.GetShaderiv(shader_obj, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		const char * message = "GLSL Compiler failed";
		const char * title = "ComputeShader";
		const char * underline = "=============";

		int log_len = 0;
		gl.GetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &log_len);

		char * log = new char[log_len];
		gl.GetShaderInfoLog(shader_obj, log_len, &log_len, log);

		gl.DeleteShader(shader_obj);

		MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

		delete[] log;
		return 0;
	}

	gl.AttachShader(program_obj, shader_obj);
	gl.LinkProgram(program_obj);

	int linked = GL_FALSE;
	gl.GetProgramiv(program_obj, GL_LINK_STATUS, &linked);

	if (!linked) {
		const char * message = "GLSL Linker failed";
		const char * title = "ComputeShader";
		const char * underline = "=============";

		int log_len = 0;
		gl.GetProgramiv(program_obj, GL_INFO_LOG_LENGTH, &log_len);

		char * log = new char[log_len];
		gl.GetProgramInfoLog(program_obj, log_len, &log_len, log);

		gl.DeleteProgram(program_obj);

		MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

		delete[] log;
		return 0;
	}

	compute_shader->shader_obj = shader_obj;
	compute_shader->program_obj = program_obj;

	Py_INCREF(compute_shader);

	int num_uniforms = 0;
	int num_uniform_blocks = 0;
	int num_subroutines = 0;
	int num_subroutine_uniforms = 0;

	gl.GetProgramiv(program_obj, GL_ACTIVE_UNIFORMS, &num_uniforms);
	gl.GetProgramiv(program_obj, GL_ACTIVE_UNIFORM_BLOCKS, &num_uniform_blocks);
	gl.GetProgramStageiv(program_obj, GL_COMPUTE_SHADER, GL_ACTIVE_SUBROUTINES, &num_subroutines);
	gl.GetProgramStageiv(program_obj, GL_COMPUTE_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &num_subroutine_uniforms);

	PyObject * uniforms_lst = PyTuple_New(num_uniforms);
	PyObject * uniform_blocks_lst = PyTuple_New(num_uniform_blocks);
	PyObject * subroutines_lst = PyTuple_New(num_subroutines);
	PyObject * subroutine_uniforms_lst = PyTuple_New(num_subroutine_uniforms);

	int uniform_counter = 0;
	for (int i = 0; i < num_uniforms; ++i) {
		int type = 0;
		int array_length = 0;
		int name_len = 0;
		char name[256];

		gl.GetActiveUniform(program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);
		int location = gl.GetUniformLocation(program_obj, name);

		clean_glsl_name(name, name_len);

		if (location < 0) {
			continue;
		}

		MGLUniform * mglo = (MGLUniform *)MGLUniform_Type.tp_alloc(&MGLUniform_Type, 0);
		mglo->type = type;
		mglo->location = location;
		mglo->array_length = array_length;
		mglo->program_obj = program_obj;
		MGLUniform_Complete(mglo, gl);

		PyObject * item = PyTuple_New(5);
		PyTuple_SET_ITEM(item, 0, (PyObject *)mglo);
		PyTuple_SET_ITEM(item, 1, PyLong_FromLong(location));
		PyTuple_SET_ITEM(item, 2, PyLong_FromLong(array_length));
		PyTuple_SET_ITEM(item, 3, PyLong_FromLong(mglo->dimension));
		PyTuple_SET_ITEM(item, 4, PyUnicode_FromStringAndSize(name, name_len));

		PyTuple_SET_ITEM(uniforms_lst, uniform_counter, item);
		++uniform_counter;
	}

	if (uniform_counter != num_uniforms) {
		_PyTuple_Resize(&uniforms_lst, uniform_counter);
	}

	for (int i = 0; i < num_uniform_blocks; ++i) {
		int size = 0;
		int name_len = 0;
		char name[256];

		gl.GetActiveUniformBlockName(program_obj, i, 256, &name_len, name);
		int index = gl.GetUniformBlockIndex(program_obj, name);
		gl.GetActiveUniformBlockiv(program_obj, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

		clean_glsl_name(name, name_len);

		MGLUniformBlock * mglo = (MGLUniformBlock *)MGLUniformBlock_Type.tp_alloc(&MGLUniformBlock_Type, 0);

		mglo->index = index;
		mglo->size = size;
		mglo->program_obj = program_obj;
		mglo->gl = &gl;

		PyObject * item = PyTuple_New(4);
		PyTuple_SET_ITEM(item, 0, (PyObject *)mglo);
		PyTuple_SET_ITEM(item, 1, PyLong_FromLong(index));
		PyTuple_SET_ITEM(item, 2, PyLong_FromLong(size));
		PyTuple_SET_ITEM(item, 3, PyUnicode_FromStringAndSize(name, name_len));

		PyTuple_SET_ITEM(uniform_blocks_lst, i, item);
	}

	int subroutine_uniforms_base = 0;
	int subroutines_base = 0;

	if (self->version_code >= 400) {
		const int shader_type[5] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
			GL_GEOMETRY_SHADER,
			GL_TESS_EVALUATION_SHADER,
			GL_TESS_CONTROL_SHADER,
		};

		for (int st = 0; st < 5; ++st) {
			int num_subroutines = 0;
			gl.GetProgramStageiv(program_obj, shader_type[st], GL_ACTIVE_SUBROUTINES, &num_subroutines);

			int num_subroutine_uniforms = 0;
			gl.GetProgramStageiv(program_obj, shader_type[st], GL_ACTIVE_SUBROUTINE_UNIFORMS, &num_subroutine_uniforms);

			for (int i = 0; i < num_subroutines; ++i) {
				int name_len = 0;
				char name[256];

				gl.GetActiveSubroutineName(program_obj, shader_type[st], i, 256, &name_len, name);
				int index = gl.GetSubroutineIndex(program_obj, shader_type[st], name);

				PyObject * item = PyTuple_New(2);
				PyTuple_SET_ITEM(item, 0, PyLong_FromLong(index));
				PyTuple_SET_ITEM(item, 1, PyUnicode_FromStringAndSize(name, name_len));
				PyTuple_SET_ITEM(subroutines_lst, subroutines_base + i, item);
			}

			for (int i = 0; i < num_subroutine_uniforms; ++i) {
				int name_len = 0;
				char name[256];

				gl.GetActiveSubroutineUniformName(program_obj, shader_type[st], i, 256, &name_len, name);
				int location = subroutine_uniforms_base + gl.GetSubroutineUniformLocation(program_obj, shader_type[st], name);
				PyTuple_SET_ITEM(subroutine_uniforms_lst, location, PyUnicode_FromStringAndSize(name, name_len));
			}

			subroutine_uniforms_base += num_subroutine_uniforms;
			subroutines_base += num_subroutines;
		}
	}

	PyObject * result = PyTuple_New(6);
	PyTuple_SET_ITEM(result, 0, (PyObject *)compute_shader);
	PyTuple_SET_ITEM(result, 1, uniforms_lst);
	PyTuple_SET_ITEM(result, 2, uniform_blocks_lst);
	PyTuple_SET_ITEM(result, 3, subroutines_lst);
	PyTuple_SET_ITEM(result, 4, subroutine_uniforms_lst);
	PyTuple_SET_ITEM(result, 5, PyLong_FromLong(compute_shader->program_obj));
	return result;
}

PyObject * MGLComputeShader_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLComputeShader * self = (MGLComputeShader *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLComputeShader_tp_dealloc(MGLComputeShader * self) {
	Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject * MGLComputeShader_run(MGLComputeShader * self, PyObject * args) {
	unsigned x;
	unsigned y;
	unsigned z;

	int args_ok = PyArg_ParseTuple(
		args,
		"III",
		&x,
		&y,
		&z
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;

	gl.UseProgram(self->program_obj);
	gl.DispatchCompute(x, y, z);

	Py_RETURN_NONE;
}

PyObject * MGLComputeShader_release(MGLComputeShader * self) {
	MGLComputeShader_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLComputeShader_tp_methods[] = {
	{"run", (PyCFunction)MGLComputeShader_run, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLComputeShader_release, METH_VARARGS, 0},
	{0},
};

PyGetSetDef MGLComputeShader_tp_getseters[] = {
	{0},
};

PyTypeObject MGLComputeShader_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.ComputeShader",                                    // tp_name
	sizeof(MGLComputeShader),                               // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLComputeShader_tp_dealloc,                // tp_dealloc
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
	MGLComputeShader_tp_methods,                            // tp_methods
	0,                                                      // tp_members
	MGLComputeShader_tp_getseters,                          // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLComputeShader_tp_new,                                // tp_new
};

void MGLComputeShader_Invalidate(MGLComputeShader * compute_shader) {
	if (Py_TYPE(compute_shader) == &MGLInvalidObject_Type) {
		return;
	}

	// TODO: decref

	const GLMethods & gl = compute_shader->context->gl;
	gl.DeleteProgram(compute_shader->program_obj);

	Py_DECREF(compute_shader->context);
	Py_TYPE(compute_shader) = &MGLInvalidObject_Type;
	Py_DECREF(compute_shader);
}
