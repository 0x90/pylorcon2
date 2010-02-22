/*
 * PyLorcon2 - Python bindings for Lorcon2 library
 * Author: 6e726d <6e726d@gmail.com>
 * Author: gutes  <egutesman@coresecurity.com>
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

