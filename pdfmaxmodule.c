
/*----------------------------------------------------------------------+
 |                                                                      |
 |      pdfmaxmodule.c -- Python module for pdfMax                      |
 |                                                                      |
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 |      Includes                                                        |
 +----------------------------------------------------------------------*/

#include <limits.h>

#include "Python.h"

#include "pdfMax.h"

/*----------------------------------------------------------------------+
 |      module                                                          |
 +----------------------------------------------------------------------*/

PyDoc_STRVAR(pdfmax_doc,
        "Fast maximum of multiple Gaussians");

/*----------------------------------------------------------------------+
 |      pdfMax()                                                        |
 +----------------------------------------------------------------------*/

PyDoc_STRVAR(pdfMax_doc,
        "pdfMax(pdfList, significance) -> mu, sigma, odds\n"
        "\n"
        "Maximum of several Gaussians\n"
        "\n"
        "Input arguments:\n"
        "       pdfList         List of n (n > 0) Gaussians, given as (mu, sigma) tuples\n"
        "       sigificance     Desired significance for the result when n > 2, meaning\n"
        "                       that either the max or the mean+3sigma of the absolute\n"
        "                       error stays within this bound, whichever is lowest.\n"
        "\n"
        "Output arguments:\n"
        "       mu, sigma       Distribution of the maximum (estimated for n > 2)\n"
        "       odds            List of odds that each input variable is the maximum\n"
        "\n"
        "Notes:\n"
        "     - Although the max distribution is generally not exactly normal, the\n"
        "       returned Gaussian preserves its first two moments.\n"
        "     - An exact result is returned for n <= 2 and `significance' ignored.\n"
        "     - For n > 2, mu, sigma and odds[] are approximated numerically within\n"
        "       the given signicance. Below 5e-12 this calculation may become unstable.\n"
);

static PyObject *
pdfmaxmodule_pdfMax(PyObject *self, PyObject *args)
{
        PyObject *pyPdfList;
        double epsilon;

        if (!PyArg_ParseTuple(args, "Od", &pyPdfList, &epsilon)) {
                return NULL;
        }

        if (!PyList_Check(pyPdfList)) {
                PyErr_SetString(PyExc_ValueError, "pdfList must be list");
                return NULL;
        }

        Py_ssize_t n = PyList_Size(pyPdfList);

        if (n > (Py_ssize_t)INT_MAX) {
                PyErr_SetString(PyExc_ValueError, "pdfList too long");
                return NULL;
        }

        double pdfList[n][2];

        for (long i=0; i<n; i++) {
                PyObject *item = PyList_GetItem(pyPdfList, i);
                if (!item) {
                        return NULL;
                }

                if (!PyTuple_Check(item)) {
                        PyErr_SetString(PyExc_ValueError, "pdfList[] must be tuple");
                        return NULL;
                }

                for (long j=0; j<2; j++) {
                        PyObject *pyFloat = PyTuple_GetItem(item, j);
                        if (!pyFloat) {
                                return NULL;
                        }

                        if (!PyFloat_Check(pyFloat)) {
                                PyErr_SetString(PyExc_ValueError, "pdfList[][] must be float");
                                return NULL;
                        }

                        double value = PyFloat_AsDouble(pyFloat);

                        if (value == -1.0 && PyErr_Occurred()) {
                                return NULL;
                        }

                        pdfList[i][j] = value;
                }
        }

        double mu;
        double sigma;
        double odds[n];

        (void) pdfMax(pdfList, n, epsilon, &mu, &sigma, odds);

        PyObject *result = PyTuple_New(3);
        if (!result) {
	        return NULL;
        }

        if (PyTuple_SetItem(result, 0, PyFloat_FromDouble(mu))) {
                Py_DECREF(result);
	        return NULL;
        }

        if (PyTuple_SetItem(result, 1, PyFloat_FromDouble(sigma))) {
                Py_DECREF(result);
	        return NULL;
        }

        PyObject *oddsList = PyList_New(n);

        if (PyTuple_SetItem(result, 2, oddsList)) {
                Py_DECREF(result);
	        return NULL;
        }

        for (long i=0; i<n; i++) {
                if (PyList_SetItem(oddsList, i, PyFloat_FromDouble(odds[i]))) {
                        Py_DECREF(result);
	                return NULL;
                }
        }

        return result;
}

/*----------------------------------------------------------------------+
 |      method table                                                    |
 +----------------------------------------------------------------------*/

static PyMethodDef pdfmaxMethods[] = {
	{ "pdfMax", pdfmaxmodule_pdfMax, METH_VARARGS, pdfMax_doc },
	{ NULL, }
};

/*----------------------------------------------------------------------+
 |      initpdfmax                                                      |
 +----------------------------------------------------------------------*/

PyMODINIT_FUNC
initpdfmax(void)
{
	PyObject *module;

        // Create the module and add the functions
        module = Py_InitModule3("pdfmax", pdfmaxMethods, pdfmax_doc);
        if (module == NULL) {
                return;
        }
}

/*----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------*/

