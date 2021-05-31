/* ====================================================================
 * Copyright (c) 2004-2010 Open Source Applications Foundation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * ====================================================================
 */

#ifndef _bases_h
#define _bases_h

#define T_OWNED    0x0001

class _wrapper {
public:
    PyObject_HEAD
    int flags;
};

class t_umemory : public _wrapper {
public:
    UMemory *object;
};

class t_uobject : public _wrapper {
public:
    UObject *object;
};


#if U_ICU_VERSION_HEX >= VERSION_HEX(55, 0, 0)

class PythonReplaceable : public Replaceable {
 public:
  explicit PythonReplaceable(PyObject *self);
  ~PythonReplaceable();

  int32_t getLength() const override;
#if U_ICU_VERSION_HEX < VERSION_HEX(59, 1, 0)
  UChar getCharAt(int32_t offset) const override;
#else
  char16_t getCharAt(int32_t offset) const override;
#endif
  UChar32 getChar32At(int32_t offset) const override;

  void extractBetween(
      int32_t start, int32_t limit, UnicodeString &target) const override;
  void handleReplaceBetween(
      int32_t start, int32_t limit, const UnicodeString &text) override;
  void copy(int32_t start, int32_t limit, int32_t dest) override;
  UBool hasMetaData() const override;

 private:
  PyObject *self_;
};

#endif

struct UNone;
typedef struct UNone UNone;

void t_umemory_dealloc(t_umemory *self);
void t_uobject_dealloc(t_uobject *self);

extern PyTypeObject UMemoryType_;
extern PyTypeObject UObjectType_;
extern PyTypeObject FormattableType_;
extern PyTypeObject PythonReplaceableType_;

PyObject *wrap_StringEnumeration(StringEnumeration *, int);
PyObject *wrap_Formattable(Formattable *, int);
PyObject *wrap_Formattable(Formattable &);
PyObject *wrap_UnicodeString(UnicodeString *, int);
PyObject *wrap_CurrencyAmount(CurrencyAmount *, int);


void _init_bases(PyObject *m);


#endif /* _bases_h */
