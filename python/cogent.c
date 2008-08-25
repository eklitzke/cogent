/* Python client lib to cogent. Can be used for in-process caching.
 *
 * Copyright Evan Klitzke, 2008
 */

#include <Python.h>
#include <cache.h>

typedef struct
{
	PyObject_HEAD
	cogent_cache *cache;
} CogentObject;

static int
cogent_init(CogentObject *self, PyObject *args)
{
	Py_ssize_t size;
	if (!PyArg_ParseTuple(args, "n", &size))
		return -1;

	self->cache = cache_init((gsize) size);
	return 0;
}

static PyObject*
cogent_store_str(CogentObject *self, PyObject *args, PyObject *kwds)
{
	const char *key;
	const char *val;
	int len;

	static char *kwlist[] = {"key", "val", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss#", kwlist, &key, &val, &len))
		return NULL;

	cache_store(self->cache, (gpointer) key, (gpointer) val, (gsize) len);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*
cogent_store(CogentObject *self, PyObject *args, PyObject *kwds)
{
	const char *key;
	PyObject *obj;

	static char *kwlist[] = {"key", "val", NULL};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &key, &obj))
		return NULL;

	/* fuck, deallocation isn't going to work... */
	Py_INCREF(obj);
	cache_store(self->cache, (gpointer) key, obj, sizeof(PyObject));

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*
cogent_get_str(CogentObject *self, PyObject *args, PyObject *kwds)
{
	const char *key;
	static char *kwlist[] = {"key", NULL};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &key))
		return NULL;

	cache_item *item = cache_fetch(self->cache, (gconstpointer) key);
	if (item == NULL)
	{
		Py_INCREF(Py_False);
		Py_INCREF(Py_None);
		return PyTuple_Pack(2, Py_False, Py_None);
	}

	Py_INCREF(Py_True);

	/* not quite right */
	return PyTuple_Pack(2, Py_True, PyString_FromStringAndSize(item->data, (Py_ssize_t) item->size));
}

static PyObject*
cogent_get(CogentObject *self, PyObject *args, PyObject *kwds)
{
	const char *key;
	static char *kwlist[] = {"key", NULL};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &key))
		return NULL;

	cache_item *item = cache_fetch(self->cache, (gconstpointer) key);
	if (item == NULL)
	{
		Py_INCREF(Py_False);
		Py_INCREF(Py_None);
		return PyTuple_Pack(2, Py_False, Py_None);
	}

	Py_INCREF(Py_True);
	return PyTuple_Pack(2, Py_True, item->data);
}

static PyObject*
cogent_size(CogentObject *self)
{
	return PyInt_FromLong((long) self->cache->cache->size);
}

static PyObject*
cogent_max_size(CogentObject *self)
{
	return PyInt_FromLong((long) self->cache->cache->max_size);
}

static PyMethodDef cogent_methods[] = {
	{ "store", (PyCFunction) cogent_store, METH_KEYWORDS, "store" },
	{ "get", (PyCFunction) cogent_get, METH_KEYWORDS, "get" },

	{ "store_str", (PyCFunction) cogent_store_str, METH_KEYWORDS, "store_str" },
	{ "get_str", (PyCFunction) cogent_get_str, METH_KEYWORDS, "get_str" },

	{ "size", (PyCFunction) cogent_size, METH_NOARGS, "size" },
	{ "max_size", (PyCFunction) cogent_max_size, METH_NOARGS, "max_size" },
	{ NULL }
};

static PyTypeObject cogent_CogentType = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,								/* ob_size */
	"cogent.Cache",					/* tp_name */
	sizeof(CogentObject),			/* tp_basicsize */
	0,								/* tp_itemsize */
	0,					/*FIXME*/			/* tp_dealloc */
	0,								/* tp_print */
	0,								/* tp_getattr */
	0,								/* tp_setattr */
	0,								/* tp_compare */
	0,								/* tp_repr */
	0,								/* tp_as_number */
	0,								/* tp_as_sequence */
	0,								/* tp_as_mapping */
	0,								/* tp_hash */
	0,								/* tp_call */
	0,								/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	PyObject_GenericSetAttr,		/* tp_setattro */
	0,								/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,				/* tp_flags*/ /* FIXME: cannot subtype */
"fixme",	/* tp_doc */
	0,								/* tp_traverse */
	0,								/* tp_clear */
	0,								/* tp_richcompare */
	0,								/* tp_weaklistoffset */
	0,								/* tp_iter */
	0,								/* tp_iternext */
	cogent_methods,					/* tp_methods */
	0,								/* tp_members */
	0,								/* tp_getset */
	0,								/* tp_base */
	0,								/* tp_dict */
	0,								/* tp_descr_get */
	0,								/* tp_descr_set */
	0,								/* tp_dictoffset */
	(initproc) cogent_init,			/* tp_init */
	0,								/* tp_alloc */
	PyType_GenericNew,				/* tp_new */
};

static PyMethodDef cogent_module_methods[] = {
	{NULL}	/* Sentinel */
};

PyMODINIT_FUNC initcogent(void)
{
	PyObject* m;

	if (PyType_Ready(&cogent_CogentType) < 0)
		return;

	m = Py_InitModule3("cogent", cogent_module_methods, "Python library to use cogent in-process.");

	Py_INCREF(&cogent_CogentType);
	PyModule_AddObject(m, "Cache", (PyObject *) &cogent_CogentType);
}
