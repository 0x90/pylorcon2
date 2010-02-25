/*
    PyLorcon2 - Python bindings for Lorcon2 library
    Copyright (C) 2010  Core Security Technologies

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
    Author: Andres Blanco (6e726d)     <6e726d@gmail.com>
    Author: Ezequiel Gutesman (gutes)  <egutesman@coresecurity.com>
*/

#include <Python.h>
#include <lorcon2/lorcon.h>

#define PASS_LORCON_EXCEPTION(call_return_value) {\
  if (call_return_value == LORCON_ENOTSUPP || call_return_value == LORCON_EGENERIC){\
    PyErr_SetString(Lorcon2Exception, lorcon_get_error(self->context));\
    return NULL;}\
}

#define CHECK_AND_RAISE_EXCEPTION(condition, text, return_value) {\
  if ((condition)){\
    PyErr_SetString(Lorcon2Exception, (text));\
    return return_value;}\
}

static PyObject *Lorcon2Exception;

typedef struct {
  PyObject_HEAD
  struct lorcon *context;
} PyLorcon2_Context;

