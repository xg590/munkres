//============================================================================
// Author      : Xiaokang Guo, Nov 23 2018, xg590@nyu.edu
// Copyright   : You are free to do anything about it
// Description : This is a python wrapper for munkres.
// 				 A PDF which details how to use it is also provided at: 
//============================================================================
#include "Python.h"
#include "numpy/arrayobject.h" 
#include "munkres.h"
static PyObject *munkres_wrapper(PyObject *self, PyObject *args) 
{
    PyObject *PyObj_cost_matrix, *PyObj_row, *PyObj_col;
    PyArrayObject *PyArrObj_cost_matrix;
    npy_intp nrow, ncol;
    int *matched_col, *matched_row;
    double *cost_matrix; 
    if (!PyArg_ParseTuple(args, "O", &PyObj_cost_matrix)) //1_PyObj_cost_matrix Get a PyObject "PyObj_cost_matrix" From PyObject "args"
      return NULL;
    PyArrObj_cost_matrix = (PyArrayObject*)PyArray_FromAny(PyObj_cost_matrix, PyArray_DescrFromType(NPY_DOUBLE), 2, 2,
    													   NPY_ARRAY_C_CONTIGUOUS|NPY_ARRAY_ALIGNED|NPY_ARRAY_FORCECAST, 0);
    if (PyArrObj_cost_matrix->nd!=2)
    {
        PyErr_SetString(PyExc_ValueError, "lap() requires a 2-D matrix");
        Py_XDECREF(PyObj_cost_matrix);
        Py_XDECREF(PyArrObj_cost_matrix);
        return NULL;
    }
    nrow = PyArrObj_cost_matrix->dimensions[0];
    ncol = PyArrObj_cost_matrix->dimensions[1];
    //get inputted matrix as a 1-D C array:
    cost_matrix = (double*) PyArray_DATA(PyArrObj_cost_matrix);
    //allocate data for the output array
    PyObj_row = PyArray_SimpleNew(1, &nrow, NPY_INT);
    PyObj_col = PyArray_SimpleNew(1, &ncol, NPY_INT);
    matched_col = (int *) PyArray_DATA(PyObj_row);
    matched_row = (int *) PyArray_DATA(PyObj_col);
    if(!(matched_col&&matched_row))
      {
        PyErr_NoMemory();
        Py_XDECREF(PyObj_cost_matrix);
        Py_XDECREF(PyArrObj_cost_matrix);
        return NULL;
      }
    munkres(cost_matrix, nrow, ncol, matched_col, matched_row);
    Py_XDECREF(PyObj_cost_matrix);
    Py_XDECREF(PyArrObj_cost_matrix);
//    printf("matched_col:");print_vector(matched_col, nrow);
    return Py_BuildValue("(NN)", PyObj_row, PyObj_col); /* Callers responsibility to decref. */
}

static char module_docstring[] = "Munkres' Assignment Problem Solver.\nIt takes 2-D matrix and returns a tuple of the form:\n(Column index matching to each row, Row index matching to each column)\n";

static PyMethodDef Methods[] = {
  {"solver",  munkres_wrapper, METH_VARARGS, module_docstring},
  {NULL, NULL, 0, NULL}        /* Sentinel (terminates structure) */
};

static struct PyModuleDef Module = 
{
    PyModuleDef_HEAD_INIT,
    "munkres",
    module_docstring,
    -1,
    Methods
};

PyMODINIT_FUNC PyInit_munkres(void) 
{
    import_array();
    return PyModule_Create(&Module);
} 
