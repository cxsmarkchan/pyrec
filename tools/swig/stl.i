// Wrap some stl containers. We wrote this file since the used swig library does not
// support stl well.
// Some of the functions (e.g. tf_vector_input_helper, _LIST_OUTPUT_TYPEMAP),
// are copied and revised from the TensorFlow source code.
// (CopyRight the TensorFlow authors. License Apache 2.0)
// see https://github.com/tensorflow/tensorflow://tensorflow/python/platform/base.i
// For these functions, We still use the original name, i.e. tf_xxxx
// These functions should be removed once SWIG supports stl well.

%{
  #include <vector>
  #include <string>
  #include <memory>

  template<class T>
  bool _PyObjAs(PyObject *pyobj, T* t) {
    T::undefined;  // You need to define specialization _PyObjAs<T>
    return false;
  }

  template<class T>
  bool _PyObjAs(PyObject *pyobj, int* t) {
    *t = PyInt_AsLong(pyobj);
    return true;
  }

  template <class T>
  bool tf_vector_input_helper(PyObject * seq, std::vector<T> * out,
                              bool (*convert)(PyObject*, T * const)) {
    PyObject *item, *it = PyObject_GetIter(seq);
    if (!it) return false;
    while ((item = PyIter_Next(it))) {
      T elem;
      bool success = convert(item, &elem);
      Py_DECREF(item);
      if (!success) {
        Py_DECREF(it);
        return false;
      }
      if (out) out->push_back(elem);
    }
    Py_DECREF(it);
    return static_cast<bool>(!PyErr_Occurred());
  } 
%}

%include "std_string.i"
%include "stdint.i"
%include "std_shared_ptr.i"

%define _LIST_OUTPUT_TYPEMAP(type, py_converter)
  %typemap(in) std::vector<type>(std::vector<type> temp) {
    if (!tf_vector_input_helper($input, &temp, _PyObjAs<type>)) {
      if (!PyErr_Occurred())
        PyErr_SetString(PyExc_TypeError, "sequence(type) expected");
      return NULL;
    }
    $1 = temp;
  }
  %typemap(in) const std::vector<type>& (std::vector<type> temp),
     const std::vector<type>* (std::vector<type> temp) {
    if (!tf_vector_input_helper($input, &temp, _PyObjAs<type>)) {
      if (!PyErr_Occurred())
        PyErr_SetString(PyExc_TypeError, "sequence(type) expected");
      return NULL;
    }
    $1 = &temp;
  }
  %typemap(in,numinputs=0)

  std::vector<type>* OUTPUT (std::vector<type> temp),
     hash_set<type>* OUTPUT (hash_set<type> temp),
     set<type>* OUTPUT (set<type> temp) {
    $1 = &temp;
  }
%enddef

_LIST_OUTPUT_TYPEMAP(int, PyInt_FromLong);
