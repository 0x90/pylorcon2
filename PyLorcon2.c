/*
 * PyLorcon2 - Python bindings for Lorcon2 library
 * Author: 6e726d <6e726d@gmail.com>
 * Author: gutes  <egutesman@coresecurity.com>
 */

#include <Python.h>
#include <lorcon2/lorcon.h>

static PyObject *Lorcon2Exception;

typedef struct {
  PyObject_HEAD
  struct lorcon *context;
} PyLorcon2_Context;

static PyObject* PyLorcon2_get_version(PyObject *self, PyObject *args)
{
  return Py_BuildValue("i", lorcon_get_version());
}

static PyObject* PyLorcon2_list_drivers(PyObject *self, PyObject *args)
{
  PyObject* list = PyList_New(0);
  lorcon_driver_t *driver_list, *driver;

  driver = driver_list = lorcon_list_drivers();

  if(driver == NULL)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to list drivers.");
    return NULL;
  }

  while(driver)
  {
    PyObject* tuple = PyTuple_New(2);

    if(tuple == NULL)
    {
      PyErr_SetString(Lorcon2Exception, "Unable to create python tuple.");
      return NULL;
    }

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

  if(!PyArg_ParseTuple(args, "s", &name))
  {
    PyErr_SetString(Lorcon2Exception, "Unable to parse arguments.");
    return NULL;
  }

  driver = lorcon_find_driver(name);

  if(driver == NULL)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to find driver.");
    return NULL;
  }

  tuple = PyTuple_New(2);

  if(tuple == NULL)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to create python tuple.");
    return NULL;
  }

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

  if(!PyArg_ParseTuple(args, "s", &iface))
  {
    PyErr_SetString(Lorcon2Exception, "Unable to parse arguments.");
    return NULL;
  }

  driver = lorcon_auto_driver(iface);

  if(driver == NULL)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to get driver with auto_driver.");
    return NULL;
  }

  tuple = PyTuple_New(2);

  if(tuple == NULL)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to create python tuple.");
    return NULL;
  }

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

  if(!PyArg_ParseTupleAndKeywords(args, kwds, "S", kwlist, &iface))
  {
    PyErr_SetString(Lorcon2Exception, "Unable to parse arguments.");
    return -1;
  }

  driver = lorcon_auto_driver(PyString_AsString(iface));

  if(driver == NULL)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to get driver with auto_driver.");
    return -1;
  }

  self->context = lorcon_create(PyString_AsString(iface), driver);

  lorcon_set_timeout(self->context, 100);

  lorcon_free_driver_list(driver);

  if(self->context == NULL)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to create lorcon context.");
    return -1;
  }

  return 0;
}

static PyObject* PyLorcon2_Context_open_inject(PyLorcon2_Context *self)
{
  if(lorcon_open_inject(self->context) < 0)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to set context to inject mode.");
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_open_monitor(PyLorcon2_Context *self)
{
  if(lorcon_open_monitor(self->context) < 0)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to set context to monitor mode.");
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_open_injmon(PyLorcon2_Context *self)
{
  if(lorcon_open_injmon(self->context) < 0)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to set context to monitor and inject mode.");
    return NULL;
  }

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
  int sent;
  unsigned char *packet;

  if(!PyArg_ParseTuple(args, "s#", &packet, &len))
  {
    PyErr_SetString(Lorcon2Exception, "Unable to parse arguments.");
    return NULL;
  }

  sent = lorcon_send_bytes(self->context, len, packet);
  if(sent < 0)
  {
    PyErr_SetString(Lorcon2Exception, "Unable to send packet.");
    return NULL;
  }

  return Py_BuildValue("i", sent);
}

static PyObject* PyLorcon2_Context_set_timeout(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  int timeout;
  static char *kwlist[] = {"timeout", NULL};

  if(!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &timeout))
  {
    PyErr_SetString(Lorcon2Exception, "Unable to parse arguments.");
    return NULL;
  }

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

  if(!PyArg_ParseTupleAndKeywords(args, kwds, "z#", kwlist, &vap))
  {
    PyErr_SetString(Lorcon2Exception, "Unable to parse arguments.");
    return NULL;
  }

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

  if(!PyArg_ParseTuple(args, "i", &channel))
  {
    PyErr_SetString(Lorcon2Exception, "Unable to parse arguments.");
    return NULL;
  }

  lorcon_set_channel(self->context, channel);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyLorcon2_Context_get_channel(PyLorcon2_Context *self)
{
  return Py_BuildValue("i", lorcon_get_channel(self->context));
}

static PyMethodDef PyLorcon2_Context_Methods[] =
{
  {"open_inject",     (PyCFunction)PyLorcon2_Context_open_inject,     METH_VARARGS, "Set context to injection mode."},
  {"open_monitor",    (PyCFunction)PyLorcon2_Context_open_monitor,    METH_VARARGS, "Set context to monitor mode."},
  {"open_injmon",     (PyCFunction)PyLorcon2_Context_open_injmon,     METH_VARARGS, "Set context to monitor and injection mode."},
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

