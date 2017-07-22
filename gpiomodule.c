#include <Python.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
//#include "gpio-utils.h"
#include <stdio.h>
#include <errno.h>

//Python API
static PyObject *
gpio_init(PyObject *self, PyObject *args)
{
  char * gpiochip;
  int nlines;
  int nvalues;
  int direction;
  PyObject * gpio_lines_list;
  PyObject * gpio_values_list;
  int fd;
  struct gpiohandle_data data;
  char *chrdev_name;
  struct gpiohandle_request req;


  //parse python arguments
  if (!PyArg_ParseTuple(args, "siO!O!", &gpiochip, &direction, &PyList_Type, &gpio_lines_list, &PyList_Type, &gpio_values_list)) {
    return NULL;
  }
  nlines = PyList_Size(gpio_lines_list);
  nvalues = PyList_Size(gpio_values_list);

  //loop through gpio_lines_list and assign to the request
  for (int i=0; i<nlines; i++)
    req.lineoffsets[i] = PyLong_AsLong(PyList_GetItem(gpio_lines_list, i));
  req.lines = nlines;


  memset(&data.values, 0, sizeof(data.values));
  for (int i=0; i<nvalues; i++){
    data.values[i] = PyLong_AsLong(PyList_GetItem(gpio_values_list, i));
  }

  //Open the chardev
  if (asprintf(&chrdev_name, "/dev/%s", gpiochip) < 0)
    return NULL;
  fd = open(chrdev_name, 0);
  free(chrdev_name);


  //Send the ioctl
  if (direction == 1){
    req.flags = GPIOHANDLE_REQUEST_OUTPUT;
    memcpy(req.default_values, &data, sizeof(req.default_values));
  } else {
    req.flags = GPIOHANDLE_REQUEST_INPUT;
  }
  strcpy(req.consumer_label, "test");

  if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req) == -1) {
    close(fd);
    return PyLong_FromLong(-errno);
  }
  return PyLong_FromLong(req.fd);
}


static PyObject *
gpio_set_values(PyObject *self, PyObject *args)
{
  int fd;
  int nvalues;
  struct gpiohandle_data data;
  PyObject * gpio_values_list;

  if (!PyArg_ParseTuple(args, "iO!", &fd, &PyList_Type, &gpio_values_list)) {
    return NULL;
  }
  nvalues = PyList_Size(gpio_values_list);

  for (int i=0; i<nvalues; i++){
    data.values[i] = PyLong_AsLong(PyList_GetItem(gpio_values_list, i));
  }
  return PyLong_FromLong(ioctl(fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data));
}

static PyObject *
gpio_get_values(PyObject *self, PyObject *args)
{
  int fd;
  int nvalues;
  struct gpiohandle_data data;
  PyObject * gpio_values_list;
  int ret;

  gpio_values_list = PyList_New(0);

  if (!PyArg_ParseTuple(args, "ii", &fd, &nvalues)) {
    return NULL;
  }

  ret = ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
  if (ret == -1)
    return NULL;

  for (int i=0; i<nvalues; i++){
    PyList_Append(gpio_values_list, PyLong_FromLong(data.values[i]));
  }

  return gpio_values_list;
}

static PyObject *
gpio_release(PyObject *self, PyObject *args)
{
  int fd;

  if (!PyArg_ParseTuple(args, "i", &fd)) {
    return NULL;
  }
  return PyLong_FromLong(close(fd));
}





static PyMethodDef gpio_Methods[] = {
 { "init", gpio_init, METH_VARARGS, "Initialize GPIO" },
 { "set_values", gpio_set_values, METH_VARARGS, "Update GPIO" },
 { "get_values", gpio_get_values, METH_VARARGS, "Query GPIO" },
 { "release", gpio_release, METH_VARARGS, "Release GPIO" },
 { NULL, NULL, 0, NULL }
};


static struct PyModuleDef gpio =
{
    PyModuleDef_HEAD_INIT,
    "gpio", /* name of module */
    "usage: Combinations.uniqueCombinations(lstSortableItems, comboSize)\n", /* module documentation, may be NULL */
    -1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    gpio_Methods
};

PyMODINIT_FUNC PyInit_gpio(void)
{
    return PyModule_Create(&gpio);
}
