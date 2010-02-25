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

#include "PyLorcon2.h"

static PyObject* PyLorcon2_get_version(PyObject *self, PyObject *args)
{
  return Py_BuildValue("i", lorcon_get_version());
}

static PyObject* PyLorcon2_list_drivers(PyObject *self, PyObject *args)
{
  PyObject* list = PyList_New(0);
  lorcon_driver_t *driver_list, *driver;

  driver = driver_list = lorcon_list_drivers();

  CHECK_AND_RAISE_EXCEPTION(driver == NULL, "Unable to list drivers", NULL);

  while(driver)
  {
    PyObject* tuple = PyTuple_New(2);

    CHECK_AND_RAISE_EXCEPTION(tuple == NULL, "Unable to create python tuple", NULL);

    PyTuple_SetItem(tuple, 0, PyString_FromString(driver->name));
    PyTuple_SetItem(tuple, 1, PyString_FromString(driver->details));

    PyList_Append(list, tuple);

    Py_DECREF(tuple);

    driver = driver->next;
  }

  lorcon_free_driver_list(driver_list);

  return list;
}

static PyObject* PyLorcon2_find_driver(PyObject *self, PyObject *args)
{
  char *name = NULL;
  PyObject* tuple = NULL;
  lorcon_driver_t *driver;

  CHECK_AND_RAISE_EXCEPTION(!PyArg_ParseTuple(args, "s", &name), "Unable to parse argunments.", NULL);

  driver = lorcon_find_driver(name);

  CHECK_AND_RAISE_EXCEPTION(driver == NULL, "Unable to find driver", NULL);

  tuple = PyTuple_New(2);

  CHECK_AND_RAISE_EXCEPTION(tuple == NULL, "Unable to create python tuple.", NULL);

  PyTuple_SetItem(tuple, 0, PyString_FromString(driver->name));
  PyTuple_SetItem(tuple, 1, PyString_FromString(driver->details));

  lorcon_free_driver_list(driver);

  return tuple;
}

static PyObject* PyLorcon2_auto_driver(PyObject *self, PyObject *args)
{
  char *iface = NULL;
  PyObject* tuple = NULL;
  lorcon_driver_t *driver;

  CHECK_AND_RAISE_EXCEPTION(!PyArg_ParseTuple(args, "s", &iface), "Unable to parse argunments.", NULL);

  driver = lorcon_auto_driver(iface);

  CHECK_AND_RAISE_EXCEPTION(driver == NULL, "Unable to get driver with auto_driver.", NULL);

  tuple = PyTuple_New(2);

  CHECK_AND_RAISE_EXCEPTION(tuple == NULL, "Unable to create python tuple.", NULL);

  PyTuple_SetItem(tuple, 0, PyString_FromString(driver->name));
  PyTuple_SetItem(tuple, 1, PyString_FromString(driver->details));

  lorcon_free_driver_list(driver);

  return tuple;
}

static PyMethodDef PyLorcon2Methods[] =
{
  {"get_version",  PyLorcon2_get_version,  METH_VARARGS, "Returns the lorcon internal version in the format YYYYMMRR(year-month-release #)."},
  {"list_drivers", PyLorcon2_list_drivers, METH_VARARGS, "Returns a list with the supported drivers."},
  {"find_driver",  PyLorcon2_find_driver,  METH_VARARGS, "Returns a tuple with the driver name and description. "},
  {"auto_driver",  PyLorcon2_auto_driver,  METH_VARARGS, "Returns a tuple with the driver name and description."},
  {NULL, NULL, 0, NULL}
};

/*
 *  PyLorcon2 Context Object
 */

static void PyLorcon2_Context_dealloc(PyLorcon2_Context *self)
{
  if(self->context != NULL)
    lorcon_free(self->context);
  self->ob_type->tp_free((PyObject*)self);
}

static PyObject* PyLorcon2_Context_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyLorcon2_Context *self;

  self = (PyLorcon2_Context *)type->tp_alloc(type, 0);

  return (PyObject *)self;
}

static int PyLorcon2_Context_init(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  PyObject *iface = NULL;
  lorcon_driver_t *driver;

  static char *kwlist[] = {"iface", NULL};

  CHECK_AND_RAISE_EXCEPTION(!PyArg_ParseTupleAndKeywords(args, kwds, "S", kwlist, &iface), "Unable to parse argunments.", -1);

  driver = lorcon_auto_driver(PyString_AsString(iface));

  CHECK_AND_RAISE_EXCEPTION(driver == NULL, "Unable to get driver with auto_driver.", -1);

  self->context = lorcon_create(PyString_AsString(iface), driver);
  lorcon_set_timeout(self->context, 100);
  lorcon_free_driver_list(driver);

  CHECK_AND_RAISE_EXCEPTION(self->context == NULL, "Unable to create lorcon context.", -1);

  return 0;
}

static PyObject* PyLorcon2_Context_open_inject(PyLorcon2_Context *self)
{
  int r;
  r = lorcon_open_inject(self->context);
  PASS_LORCON_EXCEPTION(r); 

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_open_monitor(PyLorcon2_Context *self)
{
  int r;
  
  r = lorcon_open_monitor(self->context);

  PASS_LORCON_EXCEPTION(r); 

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_open_injmon(PyLorcon2_Context *self)
{
  int r;
  
  r = lorcon_open_injmon(self->context);

  PASS_LORCON_EXCEPTION(r); 

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_close(PyLorcon2_Context *self)
{
  lorcon_close(self->context);
  printf("calling close");
  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject* PyLorcon2_Context_get_error(PyLorcon2_Context *self)
{
  return PyString_FromString(lorcon_get_error(self->context));
}

static PyObject* PyLorcon2_Context_get_capiface(PyLorcon2_Context *self)
{
  return PyString_FromString(lorcon_get_capiface(self->context));
}

static PyObject* PyLorcon2_Context_send_bytes(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  int len;
  int r;
  unsigned char *packet;

  CHECK_AND_RAISE_EXCEPTION(!PyArg_ParseTuple(args, "s#", &packet, &len), "Unable to parse argunments.", NULL);
  
  r = lorcon_send_bytes(self->context, len, packet);
  PASS_LORCON_EXCEPTION(r); 

  return Py_BuildValue("i", r);
}

static PyObject* PyLorcon2_Context_set_timeout(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  int timeout;
  static char *kwlist[] = {"timeout", NULL};

  CHECK_AND_RAISE_EXCEPTION(!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &timeout), "Unable to parse argunments.", NULL);

  lorcon_set_timeout(self->context, timeout);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_get_timeout(PyLorcon2_Context *self)
{
  return Py_BuildValue("i", lorcon_get_timeout(self->context));
}

static PyObject* PyLorcon2_Context_set_vap(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  char *vap;
  static char *kwlist[] = {"vap", NULL};

  CHECK_AND_RAISE_EXCEPTION(!PyArg_ParseTupleAndKeywords(args, kwds, "z#", kwlist, &vap), "Unable to parse argunments.", NULL);

  lorcon_set_vap(self->context, vap); 

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_get_vap(PyLorcon2_Context *self)
{
   return PyString_FromString(lorcon_get_vap(self->context));
}

static PyObject* PyLorcon2_Context_get_driver_name(PyLorcon2_Context *self)
{
   return PyString_FromString(lorcon_get_driver_name(self->context));
}

static PyObject* PyLorcon2_Context_set_channel(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  int channel;
  int r;

  CHECK_AND_RAISE_EXCEPTION(!PyArg_ParseTuple(args, "i", &channel), "Unable to parse arguments", NULL);

  r = lorcon_set_channel(self->context, channel);
  PASS_LORCON_EXCEPTION(r); 
    
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_get_channel(PyLorcon2_Context *self)
{
  int r;

  r = lorcon_get_channel(self->context);
  PASS_LORCON_EXCEPTION(r); 
  
  return Py_BuildValue("i", r);
}


static PyObject* PyLorcon2_Context_get_hwmac(PyLorcon2_Context *self)
{
    int r;
    uint8_t *mac;
    PyObject *ret;

    r = lorcon_get_hwmac(self->context, &mac);
    PASS_LORCON_EXCEPTION(r);

    ret = Py_BuildValue("[i,i,i,i,i,i]", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    free(mac);

    return ret;
}

static PyObject* PyLorcon2_Context_set_hwmac(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  PyObject *mac_list;
  Py_ssize_t list_size;
  uint8_t *mac;
  int r, i;

  CHECK_AND_RAISE_EXCEPTION(!PyArg_ParseTuple(args, "O", &mac_list), "Unable to parse arguments", NULL);
  CHECK_AND_RAISE_EXCEPTION(!PyList_Check(mac_list), "Argument must be of type List.", NULL);

  list_size = PyList_Size(mac_list);

  CHECK_AND_RAISE_EXCEPTION((list_size != 6), "MAC must be a list composed by 6 bytes.", NULL);

  mac = malloc(sizeof(uint8_t) * 6);
  for (i=0; i<6; i++)
    mac[i] = (uint8_t) PyInt_AsLong(PyList_GetItem(mac_list, i));

  r = lorcon_set_hwmac(self->context, 6, mac);
  PASS_LORCON_EXCEPTION(r);
  
  return Py_BuildValue("i", r);

}

static PyMethodDef PyLorcon2_Context_Methods[] =
{
  {"open_inject",     (PyCFunction)PyLorcon2_Context_open_inject,     METH_VARARGS, "Set context to injection mode."},
  {"open_monitor",    (PyCFunction)PyLorcon2_Context_open_monitor,    METH_VARARGS, "Set context to monitor mode."},
  {"open_injmon",     (PyCFunction)PyLorcon2_Context_open_injmon,     METH_VARARGS, "Set context to monitor and injection mode."},
  {"close",           (PyCFunction)PyLorcon2_Context_close,           METH_VARARGS, "Close context."},
  {"get_error",       (PyCFunction)PyLorcon2_Context_get_error,       METH_VARARGS, "Returns context error."},
  {"get_capiface",    (PyCFunction)PyLorcon2_Context_get_capiface,    METH_VARARGS, "Returns context interface."},
  {"send_bytes",      (PyCFunction)PyLorcon2_Context_send_bytes,      METH_VARARGS, "Send bytes."},
  {"set_timeout",     (PyCFunction)PyLorcon2_Context_set_timeout,     METH_VARARGS, "Set context timeout."},
  {"get_timeout",     (PyCFunction)PyLorcon2_Context_get_timeout,     METH_VARARGS, "Returns context timeout."},
  {"set_vap",         (PyCFunction)PyLorcon2_Context_set_vap,         METH_VARARGS, "Set context vap."},
  {"get_vap",         (PyCFunction)PyLorcon2_Context_get_vap,         METH_VARARGS, "Returns context vap."},
  {"get_driver_name", (PyCFunction)PyLorcon2_Context_get_driver_name, METH_VARARGS, "Returns context driver name."},
  {"set_channel",     (PyCFunction)PyLorcon2_Context_set_channel,     METH_VARARGS, "Set context channel."},
  {"get_channel",     (PyCFunction)PyLorcon2_Context_get_channel,     METH_VARARGS, "Returns context channel."},
  {"set_hwmac",       (PyCFunction)PyLorcon2_Context_set_hwmac,       METH_VARARGS, "Set context hardware mac."},
  {"get_hwmac",       (PyCFunction)PyLorcon2_Context_get_hwmac,       METH_VARARGS, "Returns context hardware mac."},
  {NULL, NULL, 0, NULL}
};

static PyTypeObject PyLorcon2_ContextType = {
  PyObject_HEAD_INIT(NULL)
  0,                                        /* ob_size */
  "PyLorcon2.Context",                      /* tp_name */
  sizeof(PyLorcon2_Context),                /* tp_basic_size */
  0,                                        /* tp_itemsize */
  (destructor)PyLorcon2_Context_dealloc,    /* tp_dealloc */
  0,                                        /* tp_print */
  0,                                        /* tp_getattr */
  0,                                        /* tp_setattr */
  0,                                        /* tp_compare */
  0,                                        /* tp_repr */
  0,                                        /* tp_as_number */
  0,                                        /* tp_as_sequence */
  0,                                        /* tp_as_mapping */
  0,                                        /* tp_hash */
  0,                                        /* tp_call */
  0,                                        /* tp_str */
  0,                                        /* tp_getattro */
  0,                                        /* tp_setattro */
  0,                                        /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
  "PyLorcon2 Context Object",               /* tp_doc */
  0,                                        /* tp_traverse */
  0,                                        /* tp_clear */
  0,                                        /* tp_richcompare */
  0,                                        /* tp_weaklistoffset */
  0,                                        /* tp_iter */
  0,                                        /* tp_iternext */
  PyLorcon2_Context_Methods,                /* tp_methods */
  0,                                        /* tp_members */
  0,                                        /* tp_getset */
  0,                                        /* tp_base */
  0,                                        /* tp_dict */
  0,                                        /* tp_descr_get */
  0,                                        /* tp_descr_set */
  0,                                        /* tp_dictoffset */
  (initproc)PyLorcon2_Context_init,         /* tp_init */
  0,                                        /* tp_alloc */
  PyLorcon2_Context_new                     /* tp_new */
};

PyMODINIT_FUNC initPyLorcon2(void)
{
  PyObject *m;

  if(PyType_Ready(&PyLorcon2_ContextType) < 0)
    return;

  m = Py_InitModule3("PyLorcon2", PyLorcon2Methods, "Python Wrapper Module for Lorcon2 Library.");

  if(m == NULL)
    return;

  // Lorcon2 Exception
  Lorcon2Exception = PyErr_NewException("PyLorcon2.Lorcon2Exception", NULL, NULL);
  Py_INCREF(Lorcon2Exception);
  PyModule_AddObject(m, "Lorcon2Exception", Lorcon2Exception);

  // Lorcon2 Context Object
  Py_INCREF(&PyLorcon2_ContextType);
  PyModule_AddObject(m, "Context", (PyObject *)&PyLorcon2_ContextType);
}

