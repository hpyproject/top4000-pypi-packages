Python/C API usage in Top 4000 PyPI packages
---------------------------------------------

This repository contains a copy of all the C/C++ files of the Top 4000 Python
packages which use the Python/C API. 

The goal is to provide a convenient place where to find real-world usage of
the various API functions, which will be useful when designing the equivalent
HPy version.

Some notes on this repository:

* It has been heavily inspired by
  [this similar project](https://github.com/methane/notes/tree/master/2020/wchar-cache)

* We got the list of
  [Top 4000 Packages](https://hugovk.github.io/top-pypi-packages/) as of
  2021-05-30.

* Only C/C++ files has been kept, all the others have been removed. In
  particular, we kept only files with one of the following extensions: 
  `.c, .h, .cc, .cpp, .cxx, .c++, .hpp, .hxx, .h++`.

* Cython-generated C files has been removed, since they are not interesting
  for our goals:
  
    ```
    $ find . -type f -print0 | xargs -0 grep -l "Generated by Cython" | xargs rm
    ```

* Many projects contain vendored external C libraries which do no interact at
  all with the Python/C API. To save disk space, we removed all the files
  which do NOT contain the string `Py` in it. This probably mean that there
  are still some false positive, but it allowed to reduce the total size of
  the repo from ~800 MB to ~80 MB. Using ripgrep, the files where deleted by
  the following command:

  ```
  rg --files-without-match -0 Py | xargs -0 rm
  ```

