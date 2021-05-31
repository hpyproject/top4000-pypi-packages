#include "python_util.h"

namespace devtools {
 namespace cdbg {
   class PythonCallbackEvaluator {
    public:
     PythonCallbackEvaluator(ScopedPyObject callback);
     void EvaluateCallback();

    private:
     ScopedPyObject python_callback_;
   };
 }
}
