/* Simple, synchronous python client library to access cogent. This is intended
 * mostly just to be a proof of concept. The final client library is going to
 * be fully asynchronous and most likely natively threaded.
 *
 * Copyright Evan Klitzke, 2008
 */

#include <Python.h>
#include <protocol.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glib.h>
#include <assert.h>

#define DEFAULT_PORT 22122

#define RECVBUFSZ (1 << 16)

typedef struct
{
	PyObject_HEAD
	int sock;
	void *recv_buf;
} CogentObject;

static int
cogent_init(CogentObject *self, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {"port", NULL};
	unsigned short int port = DEFAULT_PORT;
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|H", kwlist, &port))
		return -1;

	self->sock = socket(PF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	printf("servaddr.sin_port = %hu\n", ntohs(servaddr.sin_port));

	/* Set the default destination */
	if (connect(self->sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
		perror("connect");
		return -1;
	}
	printf("servaddr.sin_port = %hu\n", ntohs(servaddr.sin_port));

	struct sockaddr_in cliaddr;
	socklen_t nl = sizeof(cliaddr);
	getsockname(self->sock, (struct sockaddr *) &cliaddr, &nl);
	printf("cliaddr.sin_port = %hu\n", ntohs(cliaddr.sin_port));

	self->recv_buf = g_slice_alloc(RECVBUFSZ);

	return 0;
}

static PyObject*
cogent_get(CogentObject *self, PyObject *args, PyObject *kwds)
{
	const char *key;

	static char *kwlist[] = {"key", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &key))
		return NULL;

	size_t buf_len = 0;
	void *buf = construct_client_get(key, (uint8_t) (strlen(key) + 1), &buf_len);

	if (send(self->sock, buf, buf_len, 0) < 0)
		perror("send()");

	/* FIXME */
	int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	printf("calling recv on %d\n", recv_sock);
	size_t amt = recv(recv_sock, self->recv_buf, RECVBUFSZ, 0);
	printf("amt = %d\n", (int) amt);
	g_slice_free1(buf_len, buf);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*
cogent_set(CogentObject *self, PyObject *args, PyObject *kwds)
{
	const char *key;
	const char *val;
	int len;

	static char *kwlist[] = {"key", "val", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss#", kwlist, &key, &val, &len))
		return NULL;

	size_t buf_len = 0;
	void *buf = construct_client_set(key, (uint8_t) (strlen(key) + 1), val, len, &buf_len);

	send(self->sock, buf, buf_len, 0);
	g_slice_free1(buf_len, buf);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef cogent_methods[] = {
	{ "get", (PyCFunction) cogent_get, METH_KEYWORDS, "get(self, key)" },
	{ "set", (PyCFunction) cogent_set, METH_KEYWORDS, "set(self, key, val)" },
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
	PyModule_AddObject(m, "Client", (PyObject *) &cogent_CogentType);
}
