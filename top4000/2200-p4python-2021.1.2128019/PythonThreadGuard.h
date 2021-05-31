/*
 * PythonThreadGuard
 *
 * Copyright (c) 1997-2008, Perforce Software, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTR
 * IBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PERFORCE SOFTWARE, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * $Id: //depot/r21.1/p4-python/PythonThreadGuard.h#1 $
 *
 * Build instructions:
 *  Use Distutils - see accompanying setup.py
 *
 *      python setup.py install
 *
 */

#ifndef PYTHON_THREAD_GUARD_H
#define PYTHON_THREAD_GUARD_H


// Guard class that releases the lock in the constructor and
// re-acquires it again in the destructor

class ReleasePythonLock
{
    PyThreadState *save;
    
    public:
	ReleasePythonLock() {
            save = PyEval_SaveThread();
        }
        
	~ReleasePythonLock() {
            PyEval_RestoreThread(save);
        }
};

// Guard class that ensures the correct state of the GIL
// the Global Interpreter Lock of Python

class EnsurePythonLock
{
    PyGILState_STATE gstate;
    
    public:
        EnsurePythonLock() {
            gstate = PyGILState_Ensure();
        }
        
	~EnsurePythonLock() {
            PyGILState_Release(gstate);
        }
};
#endif // PYTHON_THREAD_GUARD_H
