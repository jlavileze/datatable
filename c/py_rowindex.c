#include "py_datatable.h"
#include "py_rowindex.h"
#include "datatable.h"



RowIndex_PyObject* RowIndexPy_from_slice(PyObject *self, PyObject *args)
{
    int64_t start, count, step;
    if (!PyArg_ParseTuple(args, "lll:RowIndex.from_slice",
                          &start, &count, &step))
        return NULL;

    if (start < 0 || count < 0) {
        PyErr_SetString(PyExc_ValueError,
                        "`start` and `count` must be nonnegative");
        return NULL;
    }

    RowIndex *rowindex = RowIndex_from_slice(start, count, step);
    RowIndex_PyObject *res = RowIndex_PyNEW();
    if (res == NULL || rowindex == NULL) goto fail;

    res->ref = rowindex;
    return res;

  fail:
    RowIndex_dealloc(rowindex);
    Py_XDECREF(res);
    return NULL;
}


RowIndex_PyObject* RowIndexPy_from_array(PyObject *self, PyObject *args)
{
    PyObject *list;
    RowIndex *rowindex = NULL;
    RowIndex_PyObject *res = NULL;
    int64_t *data = NULL;

    // Unpack arguments and check their validity
    if (!PyArg_ParseTuple(args, "O!:RowIndex.from_array", &PyList_Type, &list))
        return NULL;

    // Convert Pythonic List into a regular C array of longs
    int64_t len = (int64_t) PyList_Size(list);
    data = malloc(sizeof(int64_t) * len);
    if (data == NULL) goto fail;
    for (int64_t i = 0; i < len; ++i) {
        data[i] = PyLong_AsLong(PyList_GET_ITEM(list, i));
    }

    // Construct and return the RowIndex object
    rowindex = RowIndex_from_array(data, len);
    res = RowIndex_PyNEW();
    if (res == NULL || rowindex == NULL) goto fail;
    res->ref = rowindex;
    return res;

  fail:
    free(data);
    RowIndex_dealloc(rowindex);
    Py_XDECREF(res);
    return NULL;
}



RowIndex_PyObject* RowIndexPy_from_filter(PyObject *self, PyObject *args)
{
    DataTable_PyObject *pydt;
    RowIndex *rowindex = NULL;
    RowIndex_PyObject *res = NULL;
    void *fnptr;

    if (!PyArg_ParseTuple(args, "O!l:RowIndex.from_filter", &DataTable_PyType,
                          &pydt, &fnptr))
        return NULL;

    rowindex = RowIndex_from_filter(pydt->ref, fnptr);
    res = RowIndex_PyNEW();
    if (res == NULL || rowindex == NULL) goto fail;
    res->ref = rowindex;
    return res;

  fail:
    RowIndex_dealloc(rowindex);
    Py_XDECREF(res);
    return NULL;
}


//------ RowIndex PyObject -----------------------------------------------------

static void __dealloc__(RowIndex_PyObject *self)
{
    RowIndex_dealloc(self->ref);
    Py_TYPE(self)->tp_free((PyObject*)self);
}


PyTypeObject RowIndex_PyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_datatable.RowIndex",              /* tp_name */
    sizeof(RowIndex_PyObject),          /* tp_basicsize */
    0,                                  /* tp_itemsize */
    (destructor)__dealloc__,            /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash  */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                 /* tp_flags */
    0,                                  /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
};