
/*----------------------------------------------------------------------+
 |      pdfmaxmodule.c                                                  |
 +----------------------------------------------------------------------*/

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
        "Maximum of multiple Gaussians\n"
);

static PyObject *
pdfmaxmodule_pdfMax(PyObject *self, PyObject *args)
{
        PyObject *pyPdfList;
        double significance;

        if (!PyArg_ParseTuple(args, "Od", &pyPdfList, &significance)) {
                return NULL;
        }

        Py_ssize_t n = PyList_Size(pyPdfList);

        double pdfList[n][2];

        for (long i=0; i<n; i++) {
                PyObject *item = PyList_GetItem(pyPdfList, i);
                if (!item) {
                        return NULL;
                }

                for (long j=0; j<2; j++) {
                        PyObject *pyFloat = PyTuple_GetItem(item, j);
                        if (!pyFloat) {
                                return NULL;
                        }

                        double value = PyFloat_AsDouble(pyFloat);
                        if (value == 1.0 && PyErr_Occurred()) {
                                return NULL;
                        }

                        pdfList[i][j] = value;
                }
        }

        double mu;
        double sigma;
        double odds[n];

        (void) pdfMax(pdfList, n, significance, &mu, &sigma, odds);

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

