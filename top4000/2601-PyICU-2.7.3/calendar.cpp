/* ====================================================================
 * Copyright (c) 2004-2021 Open Source Applications Foundation.
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

#include "common.h"
#include "structmember.h"

#include "bases.h"
#include "locale.h"
#include "timezone.h"
#include "calendar.h"
#include "macros.h"

DECLARE_CONSTANTS_TYPE(UCalendarDateFields)
DECLARE_CONSTANTS_TYPE(UCalendarDaysOfWeek)
DECLARE_CONSTANTS_TYPE(UCalendarMonths)
DECLARE_CONSTANTS_TYPE(UCalendarAMPMs)


/* Calendar */

class t_calendar : public _wrapper {
public:
    Calendar *object;
};

static PyObject *t_calendar_getTime(t_calendar *self);
static PyObject *t_calendar_setTime(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_isEquivalentTo(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_equals(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_before(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_after(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_add(t_calendar *self, PyObject *args);
static PyObject *t_calendar_roll(t_calendar *self, PyObject *args);
static PyObject *t_calendar_fieldDifference(t_calendar *self, PyObject *args);
static PyObject *t_calendar_getTimeZone(t_calendar *self);
static PyObject *t_calendar_setTimeZone(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_inDaylightTime(t_calendar *self);
static PyObject *t_calendar_isLenient(t_calendar *self);
static PyObject *t_calendar_setLenient(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_getType(t_calendar *self);
static PyObject *t_calendar_getFirstDayOfWeek(t_calendar *self);
static PyObject *t_calendar_setFirstDayOfWeek(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_getMinimalDaysInFirstWeek(t_calendar *self);
static PyObject *t_calendar_setMinimalDaysInFirstWeek(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_getMinimum(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_getMaximum(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_getGreatestMinimum(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_getActualMinimum(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_getLeastMaximum(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_getActualMaximum(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_get(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_isSet(t_calendar *self, PyObject *arg);
static PyObject *t_calendar_set(t_calendar *self, PyObject *args);
static PyObject *t_calendar_clear(t_calendar *self, PyObject *args);
static PyObject *t_calendar_haveDefaultCentury(t_calendar *self);
static PyObject *t_calendar_defaultCenturyStart(t_calendar *self);
static PyObject *t_calendar_defaultCenturyStartYear(t_calendar *self);
static PyObject *t_calendar_getLocale(t_calendar *self, PyObject *args);
static PyObject *t_calendar_getLocaleID(t_calendar *self, PyObject *args);
static PyObject *t_calendar_createInstance(PyTypeObject *type, PyObject *args);
static PyObject *t_calendar_getAvailableLocales(PyTypeObject *type);
static PyObject *t_calendar_getNow(PyTypeObject *type);

static PyMethodDef t_calendar_methods[] = {
    DECLARE_METHOD(t_calendar, getTime, METH_NOARGS),
    DECLARE_METHOD(t_calendar, setTime, METH_O),
    DECLARE_METHOD(t_calendar, isEquivalentTo, METH_O),
    DECLARE_METHOD(t_calendar, equals, METH_O),
    DECLARE_METHOD(t_calendar, before, METH_O),
    DECLARE_METHOD(t_calendar, after, METH_O),
    DECLARE_METHOD(t_calendar, add, METH_VARARGS),
    DECLARE_METHOD(t_calendar, roll, METH_VARARGS),
    DECLARE_METHOD(t_calendar, fieldDifference, METH_VARARGS),
    DECLARE_METHOD(t_calendar, getTimeZone, METH_NOARGS),
    DECLARE_METHOD(t_calendar, setTimeZone, METH_O),
    DECLARE_METHOD(t_calendar, inDaylightTime, METH_NOARGS),
    DECLARE_METHOD(t_calendar, isLenient, METH_NOARGS),
    DECLARE_METHOD(t_calendar, setLenient, METH_O),
    DECLARE_METHOD(t_calendar, getType, METH_NOARGS),
    DECLARE_METHOD(t_calendar, getFirstDayOfWeek, METH_NOARGS),
    DECLARE_METHOD(t_calendar, setFirstDayOfWeek, METH_O),
    DECLARE_METHOD(t_calendar, getMinimalDaysInFirstWeek, METH_NOARGS),
    DECLARE_METHOD(t_calendar, setMinimalDaysInFirstWeek, METH_O),
    DECLARE_METHOD(t_calendar, getMinimum, METH_O),
    DECLARE_METHOD(t_calendar, getMaximum, METH_O),
    DECLARE_METHOD(t_calendar, getGreatestMinimum, METH_O),
    DECLARE_METHOD(t_calendar, getActualMinimum, METH_O),
    DECLARE_METHOD(t_calendar, getLeastMaximum, METH_O),
    DECLARE_METHOD(t_calendar, getActualMaximum, METH_O),
    DECLARE_METHOD(t_calendar, get, METH_O),
    DECLARE_METHOD(t_calendar, isSet, METH_O),
    DECLARE_METHOD(t_calendar, set, METH_VARARGS),
    DECLARE_METHOD(t_calendar, clear, METH_VARARGS),
    DECLARE_METHOD(t_calendar, haveDefaultCentury, METH_NOARGS),
    DECLARE_METHOD(t_calendar, defaultCenturyStart, METH_NOARGS),
    DECLARE_METHOD(t_calendar, defaultCenturyStartYear, METH_NOARGS),
    DECLARE_METHOD(t_calendar, getLocale, METH_VARARGS),
    DECLARE_METHOD(t_calendar, getLocaleID, METH_VARARGS),
    DECLARE_METHOD(t_calendar, createInstance, METH_VARARGS | METH_CLASS),
    DECLARE_METHOD(t_calendar, getAvailableLocales, METH_NOARGS | METH_CLASS),
    DECLARE_METHOD(t_calendar, getNow, METH_NOARGS | METH_CLASS),
    { NULL, NULL, 0, NULL }
};

DECLARE_TYPE(Calendar, t_calendar, UObject, Calendar, abstract_init, NULL)

/* GregorianCalendar */

class t_gregoriancalendar : public _wrapper {
public:
    GregorianCalendar *object;
};

static int t_gregoriancalendar_init(t_gregoriancalendar *self,
                                    PyObject *args, PyObject *kwds);
static PyObject *t_gregoriancalendar_getGregorianChange(t_gregoriancalendar *self);
static PyObject *t_gregoriancalendar_setGregorianChange(t_gregoriancalendar *self, PyObject *arg);
static PyObject *t_gregoriancalendar_isLeapYear(t_gregoriancalendar *self, PyObject *arg);

static PyMethodDef t_gregoriancalendar_methods[] = {
    DECLARE_METHOD(t_gregoriancalendar, getGregorianChange, METH_NOARGS),
    DECLARE_METHOD(t_gregoriancalendar, setGregorianChange, METH_O),
    DECLARE_METHOD(t_gregoriancalendar, isLeapYear, METH_O),
    { NULL, NULL, 0, NULL }
};

DECLARE_TYPE(GregorianCalendar, t_gregoriancalendar, Calendar,
             GregorianCalendar, t_gregoriancalendar_init, NULL)


/* Calendar */

PyObject *wrap_Calendar(Calendar *calendar)
{
    RETURN_WRAPPED_IF_ISINSTANCE(calendar, GregorianCalendar);
    return wrap_Calendar(calendar, T_OWNED);
}

static PyObject *t_calendar_getTime(t_calendar *self)
{
    UDate date;
    STATUS_CALL(date = self->object->getTime(status));
    return PyFloat_FromDouble(date / 1000.0);
}

static PyObject *t_calendar_setTime(t_calendar *self, PyObject *arg)
{
    UDate date;

    if (!parseArg(arg, "D", &date))
    {
        STATUS_CALL(self->object->setTime(date, status));
        Py_RETURN_NONE;
    }

    return PyErr_SetArgsError((PyObject *) self, "setTime", arg);
}

static PyObject *t_calendar_isEquivalentTo(t_calendar *self, PyObject *arg)
{
    Calendar *calendar;
    int b;

    if (!parseArg(arg, "P", TYPE_ID(Calendar), &calendar))
    {
        b = self->object->isEquivalentTo(*calendar);
        Py_RETURN_BOOL(b);
    }

    return PyErr_SetArgsError((PyObject *) self, "isEquivalentTo", arg);
}

static PyObject *t_calendar_equals(t_calendar *self, PyObject *arg)
{
    Calendar *calendar;
    int b;

    if (!parseArg(arg, "P", TYPE_ID(Calendar), &calendar))
    {
        STATUS_CALL(b = self->object->equals(*calendar, status));
        Py_RETURN_BOOL(b);
    }

    return PyErr_SetArgsError((PyObject *) self, "equals", arg);
}

static PyObject *t_calendar_before(t_calendar *self, PyObject *arg)
{
    Calendar *calendar;
    int b;

    if (!parseArg(arg, "P", TYPE_ID(Calendar), &calendar))
    {
        STATUS_CALL(b = self->object->before(*calendar, status));
        Py_RETURN_BOOL(b);
    }

    return PyErr_SetArgsError((PyObject *) self, "before", arg);
}

static PyObject *t_calendar_after(t_calendar *self, PyObject *arg)
{
    Calendar *calendar;
    int b;

    if (!parseArg(arg, "P", TYPE_ID(Calendar), &calendar))
    {
        STATUS_CALL(b = self->object->after(*calendar, status));
        Py_RETURN_BOOL(b);
    }

    return PyErr_SetArgsError((PyObject *) self, "after", arg);
}

static PyObject *t_calendar_add(t_calendar *self, PyObject *args)
{
    UCalendarDateFields field;
    int amount;

    if (!parseArgs(args, "ii", &field, &amount))
    {
        STATUS_CALL(self->object->add(field, amount, status));
        Py_RETURN_NONE;
    }

    return PyErr_SetArgsError((PyObject *) self, "add", args);
}

static PyObject *t_calendar_roll(t_calendar *self, PyObject *args)
{
    UCalendarDateFields field;
    int amount, up;

    if (!parseArgs(args, "iB", &field, &up))
    {
        STATUS_CALL(self->object->roll(field, (UBool) up, status));
        Py_RETURN_NONE;
    }
    if (!parseArgs(args, "ii", &field, &amount))
    {
        STATUS_CALL(self->object->roll(field, amount, status));
        Py_RETURN_NONE;
    }

    return PyErr_SetArgsError((PyObject *) self, "roll", args);
}

static PyObject *t_calendar_fieldDifference(t_calendar *self, PyObject *args)
{
    UDate date;
    UCalendarDateFields field;
    int diff;

    if (!parseArgs(args, "Di", &date, &field))
    {
        STATUS_CALL(diff = self->object->fieldDifference(date, field, status));
        return PyInt_FromLong(diff);
    }

    return PyErr_SetArgsError((PyObject *) self, "fieldDifference", args);
}

static PyObject *t_calendar_getTimeZone(t_calendar *self)
{
    const TimeZone &tz = self->object->getTimeZone();
    return wrap_TimeZone(tz);
}

static PyObject *t_calendar_setTimeZone(t_calendar *self, PyObject *arg)
{
    TimeZone *tz;

    if (!parseArg(arg, "P", TYPE_CLASSID(TimeZone), &tz))
    {
        self->object->setTimeZone(*tz); /* copied */
        Py_RETURN_NONE;
    }

    return PyErr_SetArgsError((PyObject *) self, "setTimeZone", arg);
}

static PyObject *t_calendar_inDaylightTime(t_calendar *self)
{
    int b;

    STATUS_CALL(b = self->object->inDaylightTime(status));
    Py_RETURN_BOOL(b);
}

static PyObject *t_calendar_isLenient(t_calendar *self)
{
    int b = self->object->isLenient();
    Py_RETURN_BOOL(b);
}

static PyObject *t_calendar_setLenient(t_calendar *self, PyObject *arg)
{
    int b;

    if (!parseArg(arg, "b", &b))
    {
        self->object->setLenient(b);
        Py_RETURN_NONE;
    }

    return PyErr_SetArgsError((PyObject *) self, "setLenient", arg);
}

static PyObject *t_calendar_getType(t_calendar *self)
{
    return PyString_FromString(self->object->getType());
}

static PyObject *t_calendar_getFirstDayOfWeek(t_calendar *self)
{
    return PyInt_FromLong(self->object->getFirstDayOfWeek());
}

static PyObject *t_calendar_setFirstDayOfWeek(t_calendar *self, PyObject *arg)
{
    UCalendarDaysOfWeek day;

    if (!parseArg(arg, "i", &day))
    {
        self->object->setFirstDayOfWeek(day);
        Py_RETURN_NONE;
    }

    return PyErr_SetArgsError((PyObject *) self, "setFirstDayOfWeek", arg);
}

static PyObject *t_calendar_getMinimalDaysInFirstWeek(t_calendar *self)
{
    return PyInt_FromLong(self->object->getMinimalDaysInFirstWeek());
}

static PyObject *t_calendar_setMinimalDaysInFirstWeek(t_calendar *self,
                                                      PyObject *arg)
{
    int days;

    if (!parseArg(arg, "i", &days))
    {
        self->object->setMinimalDaysInFirstWeek(days);
        Py_RETURN_NONE;
    }

    return PyErr_SetArgsError((PyObject *) self, "setMinimalDaysInFirstWeek", arg);
}

static PyObject *t_calendar_getMinimum(t_calendar *self, PyObject *arg)
{
    UCalendarDateFields field;

    if (!parseArg(arg, "i", &field))
        return PyInt_FromLong(self->object->getMinimum(field));

    return PyErr_SetArgsError((PyObject *) self, "getMinimum", arg);
}

static PyObject *t_calendar_getMaximum(t_calendar *self, PyObject *arg)
{
    UCalendarDateFields field;

    if (!parseArg(arg, "i", &field))
        return PyInt_FromLong(self->object->getMaximum(field));

    return PyErr_SetArgsError((PyObject *) self, "getMaximum", arg);
}

static PyObject *t_calendar_getGreatestMinimum(t_calendar *self, PyObject *arg)
{
    UCalendarDateFields field;

    if (!parseArg(arg, "i", &field))
        return PyInt_FromLong(self->object->getGreatestMinimum(field));

    return PyErr_SetArgsError((PyObject *) self, "getGreatestMinimum", arg);
}

static PyObject *t_calendar_getActualMinimum(t_calendar *self, PyObject *arg)
{
    UCalendarDateFields field;
    int actual;

    if (!parseArg(arg, "i", &field))
    {
        STATUS_CALL(actual = self->object->getActualMinimum(field, status));
        return PyInt_FromLong(actual);
    }

    return PyErr_SetArgsError((PyObject *) self, "getActualMinimum", arg);
}

static PyObject *t_calendar_getLeastMaximum(t_calendar *self, PyObject *arg)
{
    UCalendarDateFields field;

    if (!parseArg(arg, "i", &field))
        return PyInt_FromLong(self->object->getLeastMaximum(field));

    return PyErr_SetArgsError((PyObject *) self, "getLeastMaximum", arg);
}

static PyObject *t_calendar_getActualMaximum(t_calendar *self, PyObject *arg)
{
    UCalendarDateFields field;
    int actual;

    if (!parseArg(arg, "i", &field))
    {
        STATUS_CALL(actual = self->object->getActualMaximum(field, status));
        return PyInt_FromLong(actual);
    }

    return PyErr_SetArgsError((PyObject *) self, "getActualMaximum", arg);
}

static PyObject *t_calendar_get(t_calendar *self, PyObject *arg)
{
    UCalendarDateFields field;
    int value;

    if (!parseArg(arg, "i", &field))
    {
        STATUS_CALL(value = self->object->get(field, status));
        return PyInt_FromLong(value);
    }

    return PyErr_SetArgsError((PyObject *) self, "get", arg);
}

static PyObject *t_calendar_isSet(t_calendar *self, PyObject *arg)
{
    UCalendarDateFields field;
    int b;

    if (!parseArg(arg, "i", &field))
    {
        b = self->object->isSet(field);
        Py_RETURN_BOOL(b);
    }

    return PyErr_SetArgsError((PyObject *) self, "isSet", arg);
}

static PyObject *t_calendar_set(t_calendar *self, PyObject *args)
{
    int year, month, date, hour, minute, second;
    int field, value;
    switch (PyTuple_Size(args)) {
      case 2:
        if (!parseArgs(args, "ii", &field, &value))
        {
            self->object->set((UCalendarDateFields) field, value);
            Py_RETURN_NONE;
        }
        break;
      case 3:
        if (!parseArgs(args, "iii", &year, &month, &date))
        {
            self->object->set(year, month, date);
            Py_RETURN_NONE;
        }
        break;
      case 5:
        if (!parseArgs(args, "iiiii", &year, &month, &date, &hour, &minute))
        {
            self->object->set(year, month, date, hour, minute);
            Py_RETURN_NONE;
        }
        break;
      case 6:
        if (!parseArgs(args, "iiiiii", &year, &month, &date, &hour, &minute,
                       &second))
        {
            self->object->set(year, month, date, hour, minute, second);
            Py_RETURN_NONE;
        }
        break;
    }

    return PyErr_SetArgsError((PyObject *) self, "set", args);
}

static PyObject *t_calendar_clear(t_calendar *self, PyObject *args)
{
    UCalendarDateFields field;

    switch (PyTuple_Size(args)) {
      case 0:
        self->object->clear();
        Py_RETURN_NONE;
      case 1:
        if (!parseArgs(args, "i", &field))
        {
            self->object->clear(field);
            Py_RETURN_NONE;
        }
        break;
    }

    return PyErr_SetArgsError((PyObject *) self, "clear", args);
}

static PyObject *t_calendar_haveDefaultCentury(t_calendar *self)
{
    int b = self->object->haveDefaultCentury();
    Py_RETURN_BOOL(b);
}

static PyObject *t_calendar_defaultCenturyStart(t_calendar *self)
{
    UDate date = self->object->defaultCenturyStart();
    return PyFloat_FromDouble(date / 1000.0);
}

static PyObject *t_calendar_defaultCenturyStartYear(t_calendar *self)
{
    int year = self->object->defaultCenturyStartYear();
    return PyInt_FromLong(year);
}

static PyObject *t_calendar_getLocale(t_calendar *self, PyObject *args)
{
    ULocDataLocaleType type;
    Locale locale;

    switch (PyTuple_Size(args)) {
      case 0:
        STATUS_CALL(locale = self->object->getLocale(ULOC_VALID_LOCALE,
                                                     status));
        return wrap_Locale(locale);
      case 1:
        if (!parseArgs(args, "i", &type))
        {
            STATUS_CALL(locale = self->object->getLocale(type, status));
            return wrap_Locale(locale);
        }
        break;
    }

    return PyErr_SetArgsError((PyObject *) self, "getLocale", args);
}

static PyObject *t_calendar_getLocaleID(t_calendar *self, PyObject *args)
{
    ULocDataLocaleType type;
    const char *id;

    switch (PyTuple_Size(args)) {
      case 0:
        STATUS_CALL(id = self->object->getLocaleID(ULOC_VALID_LOCALE, status));
        return PyString_FromString(id);
      case 1:
        if (!parseArgs(args, "i", &type))
        {
            STATUS_CALL(id = self->object->getLocaleID(type, status));
            return PyString_FromString(id);
        }
        break;
    }

    return PyErr_SetArgsError((PyObject *) self, "getLocaleID", args);
}

static PyObject *t_calendar_createInstance(PyTypeObject *type, PyObject *args)
{
    TimeZone *tz;
    Locale *locale;
    Calendar *calendar;

    switch (PyTuple_Size(args)) {
      case 0:
      {
          STATUS_CALL(calendar = Calendar::createInstance(status));
          return wrap_Calendar(calendar);
      }
      case 1:
        if (!parseArgs(args, "P", TYPE_CLASSID(TimeZone), &tz))
        {
            STATUS_CALL(calendar = Calendar::createInstance(*tz, status));
            return wrap_Calendar(calendar);
        }
        if (!parseArgs(args, "P", TYPE_CLASSID(Locale), &locale))
        {
            STATUS_CALL(calendar = Calendar::createInstance(*locale, status));
            return wrap_Calendar(calendar);
        }
        break;
      case 2:
        if (!parseArgs(args, "PP",
                       TYPE_CLASSID(TimeZone),
                       TYPE_CLASSID(Locale),
                       &tz, &locale))
        {
            STATUS_CALL(calendar = Calendar::createInstance(*tz, *locale, status));
            return wrap_Calendar(calendar);
        }
        break;
    }

    return PyErr_SetArgsError(type, "createInstance", args);
}

static PyObject *t_calendar_getAvailableLocales(PyTypeObject *type)
{
    int count;
    const Locale *locales = Calendar::getAvailableLocales(count);
    PyObject *dict = PyDict_New();

    for (int32_t i = 0; i < count; i++) {
        Locale *locale = (Locale *) locales + i;
        PyObject *obj = wrap_Locale(locale, 0);
        PyDict_SetItemString(dict, locale->getName(), obj);
	Py_DECREF(obj);
    }

    return dict;
}

static PyObject *t_calendar_getNow(PyTypeObject *type)
{
    UDate date = Calendar::getNow();
    return PyFloat_FromDouble(date / 1000.0);
}

static PyObject *t_calendar_str(t_calendar *self)
{
    UDate date;
    Locale locale;
    UnicodeString u;

    STATUS_CALL(date = self->object->getTime(status));
    STATUS_CALL(locale = self->object->getLocale(ULOC_VALID_LOCALE, status));

    DateFormat *df = DateFormat::createDateTimeInstance(DateFormat::kDefault,
                                                        DateFormat::kDefault,
                                                        locale);
    df->format(date, u);
    delete df;

    return PyUnicode_FromUnicodeString(&u);
}

DEFINE_RICHCMP(Calendar, t_calendar)


/* GregorianCalendar */

static int t_gregoriancalendar_init(t_gregoriancalendar *self,
                                    PyObject *args, PyObject *kwds)
{
    SimpleTimeZone *tz;
    Locale *locale;
    int year, month, date, hour, minute, second;
    GregorianCalendar *calendar;

    switch (PyTuple_Size(args)) {
      case 0:
        INT_STATUS_CALL(calendar = new GregorianCalendar(status));
        self->object = calendar;
        self->flags = T_OWNED;
        break;
      case 1:
        if (!parseArgs(args, "P", TYPE_CLASSID(TimeZone), &tz))
        {
            INT_STATUS_CALL(calendar = new GregorianCalendar(*tz, status));
            self->object = calendar;
            self->flags = T_OWNED;
            break;
        }
        if (!parseArgs(args, "P", TYPE_CLASSID(Locale), &locale))
        {
            INT_STATUS_CALL(calendar = new GregorianCalendar(*locale, status));
            self->object = calendar;
            self->flags = T_OWNED;
            break;
        }
        PyErr_SetArgsError((PyObject *) self, "__init__", args);
        return -1;
      case 2:
        if (!parseArgs(args, "PP",
                       TYPE_CLASSID(TimeZone),
                       TYPE_CLASSID(Locale),
                       &tz, &locale))
        {
            INT_STATUS_CALL(calendar = new GregorianCalendar(*tz, *locale, status));
            self->object = calendar;
            self->flags = T_OWNED;
            break;
        }
        PyErr_SetArgsError((PyObject *) self, "__init__", args);
        return -1;
      case 3:
        if (!parseArgs(args, "iii", &year, &month, &date))
        {
            INT_STATUS_CALL(calendar = new GregorianCalendar(year, month, date, status));
            self->object = calendar;
            self->flags = T_OWNED;
            break;
        }
        PyErr_SetArgsError((PyObject *) self, "__init__", args);
        return -1;
      case 5:
        if (!parseArgs(args, "iiiii", &year, &month, &date, &hour, &minute))
        {
            INT_STATUS_CALL(calendar = new GregorianCalendar(year, month, date, hour, minute, status));
            self->object = calendar;
            self->flags = T_OWNED;
            break;
        }
        PyErr_SetArgsError((PyObject *) self, "__init__", args);
        return -1;
      case 6:
        if (!parseArgs(args, "iiiiii", &year, &month, &date, &hour, &minute,
                       &second))
        {
            INT_STATUS_CALL(calendar = new GregorianCalendar(year, month, date, hour, minute, second, status));
            self->object = calendar;
            self->flags = T_OWNED;
            break;
        }
        PyErr_SetArgsError((PyObject *) self, "__init__", args);
        return -1;
      default:
        PyErr_SetArgsError((PyObject *) self, "__init__", args);
        return -1;
    }

    if (self->object)
        return 0;

    return -1;
}

static PyObject *t_gregoriancalendar_getGregorianChange(t_gregoriancalendar *self)
{
    UDate date = self->object->getGregorianChange();
    return PyFloat_FromDouble(date / 1000.0);
}

static PyObject *t_gregoriancalendar_setGregorianChange(t_gregoriancalendar *self, PyObject *arg)
{
    UDate date;

    if (!parseArg(arg, "D", &date))
    {
        STATUS_CALL(self->object->setGregorianChange(date, status));
        Py_RETURN_NONE;
    }

    return PyErr_SetArgsError((PyObject *) self, "setGregorianChange", arg);
}

static PyObject *t_gregoriancalendar_isLeapYear(t_gregoriancalendar *self, PyObject *arg)
{
    int year, b;

    if (!parseArg(arg, "i", &year))
    {
        b = self->object->isLeapYear(year);
        Py_RETURN_BOOL(b);
    }

    return PyErr_SetArgsError((PyObject *) self, "isLeapYear", arg);
}


void _init_calendar(PyObject *m)
{
    CalendarType_.tp_str = (reprfunc) t_calendar_str;
    CalendarType_.tp_richcompare = (richcmpfunc) t_calendar_richcmp;

    INSTALL_CONSTANTS_TYPE(UCalendarDateFields, m);
    INSTALL_CONSTANTS_TYPE(UCalendarDaysOfWeek, m);
    INSTALL_CONSTANTS_TYPE(UCalendarMonths, m);
    INSTALL_CONSTANTS_TYPE(UCalendarAMPMs, m);

    INSTALL_TYPE(Calendar, m);
    REGISTER_TYPE(GregorianCalendar, m);

    INSTALL_ENUM(UCalendarDateFields, "ERA", UCAL_ERA);
    INSTALL_ENUM(UCalendarDateFields, "YEAR", UCAL_YEAR);
    INSTALL_ENUM(UCalendarDateFields, "MONTH", UCAL_MONTH);
    INSTALL_ENUM(UCalendarDateFields, "WEEK_OF_YEAR", UCAL_WEEK_OF_YEAR);
    INSTALL_ENUM(UCalendarDateFields, "WEEK_OF_MONTH", UCAL_WEEK_OF_MONTH);
    INSTALL_ENUM(UCalendarDateFields, "DATE", UCAL_DATE);
    INSTALL_ENUM(UCalendarDateFields, "DAY_OF_YEAR", UCAL_DAY_OF_YEAR);
    INSTALL_ENUM(UCalendarDateFields, "DAY_OF_WEEK", UCAL_DAY_OF_WEEK);
    INSTALL_ENUM(UCalendarDateFields, "DAY_OF_WEEK_IN_MONTH", UCAL_DAY_OF_WEEK_IN_MONTH);
    INSTALL_ENUM(UCalendarDateFields, "AM_PM", UCAL_AM_PM);
    INSTALL_ENUM(UCalendarDateFields, "HOUR", UCAL_HOUR);
    INSTALL_ENUM(UCalendarDateFields, "HOUR_OF_DAY", UCAL_HOUR_OF_DAY);
    INSTALL_ENUM(UCalendarDateFields, "MINUTE", UCAL_MINUTE);
    INSTALL_ENUM(UCalendarDateFields, "SECOND", UCAL_SECOND);
    INSTALL_ENUM(UCalendarDateFields, "MILLISECOND", UCAL_MILLISECOND);
    INSTALL_ENUM(UCalendarDateFields, "ZONE_OFFSET", UCAL_ZONE_OFFSET);
    INSTALL_ENUM(UCalendarDateFields, "DST_OFFSET", UCAL_DST_OFFSET);
    INSTALL_ENUM(UCalendarDateFields, "YEAR_WOY", UCAL_YEAR_WOY);
    INSTALL_ENUM(UCalendarDateFields, "DOW_LOCAL", UCAL_DOW_LOCAL);
    INSTALL_ENUM(UCalendarDateFields, "EXTENDED_YEAR", UCAL_EXTENDED_YEAR);
    INSTALL_ENUM(UCalendarDateFields, "JULIAN_DAY", UCAL_JULIAN_DAY);
    INSTALL_ENUM(UCalendarDateFields, "MILLISECONDS_IN_DAY", UCAL_MILLISECONDS_IN_DAY);
    INSTALL_ENUM(UCalendarDateFields, "DAY_OF_MONTH", UCAL_DAY_OF_MONTH);

    INSTALL_ENUM(UCalendarDaysOfWeek, "SUNDAY", UCAL_SUNDAY);
    INSTALL_ENUM(UCalendarDaysOfWeek, "MONDAY", UCAL_MONDAY);
    INSTALL_ENUM(UCalendarDaysOfWeek, "TUESDAY", UCAL_TUESDAY);
    INSTALL_ENUM(UCalendarDaysOfWeek, "WEDNESDAY", UCAL_WEDNESDAY);
    INSTALL_ENUM(UCalendarDaysOfWeek, "THURSDAY", UCAL_THURSDAY);
    INSTALL_ENUM(UCalendarDaysOfWeek, "FRIDAY", UCAL_FRIDAY);
    INSTALL_ENUM(UCalendarDaysOfWeek, "SATURDAY", UCAL_SATURDAY);

    INSTALL_ENUM(UCalendarMonths, "JANUARY", UCAL_JANUARY);
    INSTALL_ENUM(UCalendarMonths, "FEBRUARY", UCAL_FEBRUARY);
    INSTALL_ENUM(UCalendarMonths, "MARCH", UCAL_MARCH);
    INSTALL_ENUM(UCalendarMonths, "APRIL", UCAL_APRIL);
    INSTALL_ENUM(UCalendarMonths, "MAY", UCAL_MAY);
    INSTALL_ENUM(UCalendarMonths, "JUNE", UCAL_JUNE);
    INSTALL_ENUM(UCalendarMonths, "JULY", UCAL_JULY);
    INSTALL_ENUM(UCalendarMonths, "AUGUST", UCAL_AUGUST);
    INSTALL_ENUM(UCalendarMonths, "SEPTEMBER", UCAL_SEPTEMBER);
    INSTALL_ENUM(UCalendarMonths, "OCTOBER", UCAL_OCTOBER);
    INSTALL_ENUM(UCalendarMonths, "NOVEMBER", UCAL_NOVEMBER);
    INSTALL_ENUM(UCalendarMonths, "DECEMBER", UCAL_DECEMBER);
    INSTALL_ENUM(UCalendarMonths, "UNDECIMBER", UCAL_UNDECIMBER);

    INSTALL_ENUM(UCalendarAMPMs, "AM", UCAL_AM);
    INSTALL_ENUM(UCalendarAMPMs, "PM", UCAL_PM);

    INSTALL_STATIC_INT(Calendar, ERA);
    INSTALL_STATIC_INT(Calendar, YEAR);
    INSTALL_STATIC_INT(Calendar, MONTH);
    INSTALL_STATIC_INT(Calendar, WEEK_OF_YEAR);
    INSTALL_STATIC_INT(Calendar, WEEK_OF_MONTH);
    INSTALL_STATIC_INT(Calendar, DATE);
    INSTALL_STATIC_INT(Calendar, DAY_OF_YEAR);
    INSTALL_STATIC_INT(Calendar, DAY_OF_WEEK);
    INSTALL_STATIC_INT(Calendar, DAY_OF_WEEK_IN_MONTH);
    INSTALL_STATIC_INT(Calendar, AM_PM);
    INSTALL_STATIC_INT(Calendar, HOUR);
    INSTALL_STATIC_INT(Calendar, HOUR_OF_DAY);
    INSTALL_STATIC_INT(Calendar, MINUTE);
    INSTALL_STATIC_INT(Calendar, SECOND);
    INSTALL_STATIC_INT(Calendar, MILLISECOND);
    INSTALL_STATIC_INT(Calendar, ZONE_OFFSET);
    INSTALL_STATIC_INT(Calendar, DST_OFFSET);
    INSTALL_STATIC_INT(Calendar, YEAR_WOY);
    INSTALL_STATIC_INT(Calendar, DOW_LOCAL);

    INSTALL_STATIC_INT(Calendar, SUNDAY);
    INSTALL_STATIC_INT(Calendar, MONDAY);
    INSTALL_STATIC_INT(Calendar, TUESDAY);
    INSTALL_STATIC_INT(Calendar, WEDNESDAY);
    INSTALL_STATIC_INT(Calendar, THURSDAY);
    INSTALL_STATIC_INT(Calendar, FRIDAY);
    INSTALL_STATIC_INT(Calendar, SATURDAY);

    INSTALL_STATIC_INT(Calendar, JANUARY);
    INSTALL_STATIC_INT(Calendar, FEBRUARY);
    INSTALL_STATIC_INT(Calendar, MARCH);
    INSTALL_STATIC_INT(Calendar, APRIL);
    INSTALL_STATIC_INT(Calendar, MAY);
    INSTALL_STATIC_INT(Calendar, JUNE);
    INSTALL_STATIC_INT(Calendar, JULY);
    INSTALL_STATIC_INT(Calendar, AUGUST);
    INSTALL_STATIC_INT(Calendar, SEPTEMBER);
    INSTALL_STATIC_INT(Calendar, OCTOBER);
    INSTALL_STATIC_INT(Calendar, NOVEMBER);
    INSTALL_STATIC_INT(Calendar, DECEMBER);
    INSTALL_STATIC_INT(Calendar, UNDECIMBER);

    INSTALL_STATIC_INT(Calendar, AM);
    INSTALL_STATIC_INT(Calendar, PM);

    INSTALL_STATIC_INT(GregorianCalendar, BC);
    INSTALL_STATIC_INT(GregorianCalendar, AD);
}
