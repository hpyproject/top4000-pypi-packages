#include "Types.hpp"

PyObject * MGLAttribute_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLAttribute * self = (MGLAttribute *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLAttribute_tp_dealloc(MGLAttribute * self) {
	MGLAttribute_Type.tp_free((PyObject *)self);
}

PyTypeObject MGLAttribute_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Attribute",                                        // tp_name
	sizeof(MGLAttribute),                                   // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLAttribute_tp_dealloc,                    // tp_dealloc
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
	0,                                                      // tp_methods
	0,                                                      // tp_members
	0,                                                      // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLAttribute_tp_new,                                    // tp_new
};

void MGLAttribute_Invalidate(MGLAttribute * attribute) {
	if (Py_TYPE(attribute) == &MGLInvalidObject_Type) {
		return;
	}

	// TODO: decref

	Py_TYPE(attribute) = &MGLInvalidObject_Type;
	Py_DECREF(attribute);
}

void MGLAttribute_Complete(MGLAttribute * self, const GLMethods & gl) {
	switch (self->type) {
		case GL_INT:
			self->dimension = 1;

			self->scalar_type = GL_INT;

			self->rows_length = self->array_length * 1;
			self->row_length = 1;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;

			self->normalizable = false;
			self->shape = 'i';
			break;

		case GL_INT_VEC2:
			self->dimension = 2;

			self->scalar_type = GL_INT;

			self->rows_length = self->array_length * 1;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;

			self->normalizable = false;
			self->shape = 'i';
			break;

		case GL_INT_VEC3:
			self->dimension = 3;

			self->scalar_type = GL_INT;

			self->rows_length = self->array_length * 1;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;

			self->normalizable = false;
			self->shape = 'i';
			break;

		case GL_INT_VEC4:
			self->dimension = 4;

			self->scalar_type = GL_INT;

			self->rows_length = self->array_length * 1;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;

			self->normalizable = false;
			self->shape = 'i';
			break;

		case GL_UNSIGNED_INT:
			self->dimension = 1;

			self->scalar_type = GL_UNSIGNED_INT;

			self->rows_length = self->array_length * 1;
			self->row_length = 1;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;

			self->normalizable = false;
			self->shape = 'I';
			break;

		case GL_UNSIGNED_INT_VEC2:
			self->dimension = 2;

			self->scalar_type = GL_UNSIGNED_INT;

			self->rows_length = self->array_length * 1;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;

			self->normalizable = false;
			self->shape = 'I';
			break;

		case GL_UNSIGNED_INT_VEC3:
			self->dimension = 3;

			self->scalar_type = GL_UNSIGNED_INT;

			self->rows_length = self->array_length * 1;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;

			self->normalizable = false;
			self->shape = 'I';
			break;

		case GL_UNSIGNED_INT_VEC4:
			self->dimension = 4;

			self->scalar_type = GL_UNSIGNED_INT;

			self->rows_length = self->array_length * 1;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribIPointer;

			self->normalizable = false;
			self->shape = 'I';
			break;

		case GL_FLOAT:
			self->dimension = 1;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 1;
			self->row_length = 1;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_VEC2:
			self->dimension = 2;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 1;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_VEC3:
			self->dimension = 3;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 1;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_VEC4:
			self->dimension = 4;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 1;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_DOUBLE:
			self->dimension = 1;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 1;
			self->row_length = 1;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_VEC2:
			self->dimension = 2;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 1;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_VEC3:
			self->dimension = 3;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 1;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_VEC4:
			self->dimension = 4;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 1;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_FLOAT_MAT2:
			self->dimension = 4;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 2;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_MAT2x3:
			self->dimension = 6;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 2;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_MAT2x4:
			self->dimension = 8;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 2;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_MAT3x2:
			self->dimension = 6;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 3;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_MAT3:
			self->dimension = 9;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 3;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_MAT3x4:
			self->dimension = 12;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 3;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_MAT4x2:
			self->dimension = 8;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 4;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_MAT4x3:
			self->dimension = 12;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 4;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_FLOAT_MAT4:
			self->dimension = 16;

			self->scalar_type = GL_FLOAT;

			self->rows_length = self->array_length * 4;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = true;
			self->shape = 'f';
			break;

		case GL_DOUBLE_MAT2:
			self->dimension = 4;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 2;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_MAT2x3:
			self->dimension = 6;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 2;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_MAT2x4:
			self->dimension = 8;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 2;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_MAT3x2:
			self->dimension = 6;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 3;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_MAT3:
			self->dimension = 9;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 3;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_MAT3x4:
			self->dimension = 12;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 3;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_MAT4x2:
			self->dimension = 8;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 4;
			self->row_length = 2;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_MAT4x3:
			self->dimension = 12;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 4;
			self->row_length = 3;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		case GL_DOUBLE_MAT4:
			self->dimension = 16;

			self->scalar_type = GL_DOUBLE;

			self->rows_length = self->array_length * 4;
			self->row_length = 4;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribLPointer;

			self->normalizable = false;
			self->shape = 'd';
			break;

		default:
			self->dimension = 1;

			self->scalar_type = 0;

			self->rows_length = self->array_length * 1;
			self->row_length = 1;

			self->gl_attrib_ptr_proc = (void *)gl.VertexAttribPointer;

			self->normalizable = false;
			self->shape = 0;
			break;
	}
}
