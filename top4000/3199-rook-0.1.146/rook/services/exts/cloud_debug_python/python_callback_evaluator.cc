#include "python_callback_evaluator.h"

namespace devtools {
 namespace cdbg {
  PythonCallbackEvaluator::PythonCallbackEvaluator(
      ScopedPyObject callback) : python_callback_(callback)
      {
  }

  void PythonCallbackEvaluator::EvaluateCallback() {
   ScopedPyObject callback_args(PyTuple_New(0));

   ScopedPyObject result(
       PyObject_Call(python_callback_.get(), callback_args.get(), nullptr));
   ClearPythonException();
  }
 }
}
