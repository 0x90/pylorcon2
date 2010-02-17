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

/*
 *  PyLorcon2 Module Functions
 *   - lorcon_get_version
 *   - lorcon_list_drivers
 *   - lorcon_find_driver
 *   - lorcon_auto_driver
 */

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
    return NULL;

  while(driver)
  {
    PyObject* tuple = PyTuple_New(2);

    if(tuple == NULL)
      return NULL;

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
    return NULL;

  driver = lorcon_find_driver(name);

  if(driver == NULL)
    return Py_None;

  tuple = PyTuple_New(2);

  if(tuple == NULL)
    return NULL;

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
    return NULL;

  driver = lorcon_auto_driver(iface);

  if(driver == NULL)
    return Py_None;

  tuple = PyTuple_New(2);

  if(tuple == NULL)
    return NULL;

  PyTuple_SetItem(tuple, 0, PyString_FromString(driver->name));
  PyTuple_SetItem(tuple, 1, PyString_FromString(driver->details));

  lorcon_free_driver_list(driver);

  return tuple;
}

static PyMethodDef PyLorcon2Methods[] =
{
  {"get_version",  PyLorcon2_get_version,  METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"list_drivers", PyLorcon2_list_drivers, METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"find_driver",  PyLorcon2_find_driver,  METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"auto_driver",  PyLorcon2_auto_driver,  METH_VARARGS, "TODO: Get Lorcon2 version."},
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
    return -1;

  driver = lorcon_auto_driver(PyString_AsString(iface));

  if(driver == NULL)
    return -1;

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
    PyErr_SetString(Lorcon2Exception, "Unable to set injection mode.");

  return Py_None;
}

static PyObject* PyLorcon2_Context_open_monitor(PyLorcon2_Context *self)
{
  if(lorcon_open_monitor(self->context) < 0)
    PyErr_SetString(Lorcon2Exception, "Unable to set monitor mode.");

  return Py_None;
}

static PyObject* PyLorcon2_Context_open_injmon(PyLorcon2_Context *self)
{
  if(lorcon_open_injmon(self->context) < 0)
    PyErr_SetString(Lorcon2Exception, "Unable to set injection and monitor mode.");

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
  unsigned char *packet;

  if(!PyArg_ParseTuple(args, "s#", &packet, &len))
    return NULL;

  if(lorcon_send_bytes(self->context, len, packet) < 0)
    return NULL;

  return Py_None;
}

static PyObject* PyLorcon2_Context_set_timeout(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"timeout", NULL};
  int timeout;

  if(!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &timeout))
    return NULL;

  lorcon_set_timeout(self->context, timeout);

  return Py_None;
}

static PyObject* PyLorcon2_Context_get_timeout(PyLorcon2_Context *self)
{
  return Py_BuildValue("i", lorcon_get_timeout(self->context));
}

static PyObject* PyLorcon2_Context_set_vap(PyLorcon2_Context *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"vap", NULL};
  char *vap;

  if(!PyArg_ParseTupleAndKeywords(args, kwds, "z#", kwlist, &vap))
    return NULL;

  lorcon_set_vap(self->context, vap); 

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
    return NULL;

  lorcon_set_channel(self->context, channel);

  return Py_None;

}

static PyObject* PyLorcon2_Context_get_channel(PyLorcon2_Context *self)
{
  return Py_BuildValue("i", lorcon_get_channel(self->context));
}

static PyMethodDef PyLorcon2_Context_Methods[] =
{
  {"open_inject",   (PyCFunction)PyLorcon2_Context_open_inject,   METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"open_monitor",  (PyCFunction)PyLorcon2_Context_open_monitor,  METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"open_injmon",   (PyCFunction)PyLorcon2_Context_open_injmon,   METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"get_error",     (PyCFunction)PyLorcon2_Context_get_error,     METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"get_capiface",  (PyCFunction)PyLorcon2_Context_get_capiface,  METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"send_bytes",    (PyCFunction)PyLorcon2_Context_send_bytes,    METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"set_timeout",   (PyCFunction)PyLorcon2_Context_set_timeout,   METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"get_timeout",   (PyCFunction)PyLorcon2_Context_get_timeout,   METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"set_vap",       (PyCFunction)PyLorcon2_Context_set_vap,       METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"get_vap",       (PyCFunction)PyLorcon2_Context_get_vap,       METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"get_driver_name", (PyCFunction)PyLorcon2_Context_get_driver_name,       METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"set_channel",   (PyCFunction)PyLorcon2_Context_set_channel,   METH_VARARGS, "TODO: Get Lorcon2 version."},
  {"get_channel",   (PyCFunction)PyLorcon2_Context_get_channel,   METH_VARARGS, "TODO: Get Lorcon2 version."},
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

  m = Py_InitModule3("PyLorcon2", PyLorcon2Methods, "Lorco2 Python Wrapper Module.");

  if(m == NULL)
    return;

  Py_INCREF(&PyLorcon2_ContextType);
  PyModule_AddObject(m, "Context", (PyObject *)&PyLorcon2_ContextType);
}

