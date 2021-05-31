#pragma once

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

inline void clean_glsl_name(char * name, int & name_len) {
	if (name_len && name[name_len - 1] == ']') {
		name_len -= 1;
		while (name_len && name[name_len] != '[') {
			name_len -= 1;
		}
	}
	name[name_len] = 0;
}

inline int swizzle_from_char(char c) {
	switch (c) {
		case 'R':
		case 'r':
			return GL_RED;

		case 'G':
		case 'g':
			return GL_GREEN;

		case 'B':
		case 'b':
			return GL_BLUE;

		case 'A':
		case 'a':
			return GL_ALPHA;

		case '0':
			return GL_ZERO;

		case '1':
			return GL_ONE;
	}

	return -1;
}

inline char char_from_swizzle(int c) {
	switch (c) {
		case GL_RED:
			return 'R';

		case GL_GREEN:
			return 'G';

		case GL_BLUE:
			return 'B';

		case GL_ALPHA:
			return 'A';

		case GL_ZERO:
			return '0';

		case GL_ONE:
			return '1';
	}

	return '?';
}

inline int compare_func_from_string(const char * str) {
	if (!str[0] || (str[1] && str[2])) {
		return 0;
	}

	switch (str[0] * 256 + str[1]) {
		case ('<' * 256 + '='):
			return GL_LEQUAL;

		case ('<' * 256):
			return GL_LESS;

		case ('>' * 256 + '='):
			return GL_GEQUAL;

		case ('>' * 256):
			return GL_GREATER;

		case ('=' * 256 + '='):
			return GL_EQUAL;

		case ('!' * 256 + '='):
			return GL_NOTEQUAL;

		case ('0' * 256):
			return GL_NEVER;

		case ('1' * 256):
			return GL_ALWAYS;

		default:
			return 0;
	}
}

inline PyObject * compare_func_to_string(int func) {
	switch (func) {
		case GL_LEQUAL: {
			static PyObject * res_lequal = PyUnicode_FromString("<=");
			Py_INCREF(res_lequal);
			return res_lequal;
		}

		case GL_LESS: {
			static PyObject * res_less = PyUnicode_FromString("<");
			Py_INCREF(res_less);
			return res_less;
		}

		case GL_GEQUAL: {
			static PyObject * res_gequal = PyUnicode_FromString(">=");
			Py_INCREF(res_gequal);
			return res_gequal;
		}

		case GL_GREATER: {
			static PyObject * res_greater = PyUnicode_FromString(">");
			Py_INCREF(res_greater);
			return res_greater;
		}

		case GL_EQUAL: {
			static PyObject * res_equal = PyUnicode_FromString("==");
			Py_INCREF(res_equal);
			return res_equal;
		}

		case GL_NOTEQUAL: {
			static PyObject * res_notequal = PyUnicode_FromString("!=");
			Py_INCREF(res_notequal);
			return res_notequal;
		}

		case GL_NEVER: {
			static PyObject * res_never = PyUnicode_FromString("0");
			Py_INCREF(res_never);
			return res_never;
		}

		case GL_ALWAYS: {
			static PyObject * res_always = PyUnicode_FromString("1");
			Py_INCREF(res_always);
			return res_always;
		}

		default: {
			static PyObject * res_unk = PyUnicode_FromString("?");
			Py_INCREF(res_unk);
			return res_unk;
		}
	}
}

inline PyObject * tuple2(PyObject * a, PyObject * b) {
	PyObject * res = PyTuple_New(2);
	PyTuple_SET_ITEM(res, 0, a);
	PyTuple_SET_ITEM(res, 1, b);
	return res;
}

inline PyObject * tuple3(PyObject * a, PyObject * b, PyObject * c) {
	PyObject * res = PyTuple_New(3);
	PyTuple_SET_ITEM(res, 0, a);
	PyTuple_SET_ITEM(res, 1, b);
	PyTuple_SET_ITEM(res, 2, c);
	return res;
}
