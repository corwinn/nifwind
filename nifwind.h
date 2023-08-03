/**** BEGIN LICENSE BLOCK ****

BSD 3-Clause License

Copyright (c) 2023, the wind.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**** END LICENCE BLOCK ****/

#ifndef _NIFWIND_H_
#define _NIFWIND_H_

#define NIFWIND_PROJECT "nifwind 0.1"

#define NIFWIND_NS nifwind
#define NIFWIND_NAMESPACE namespace NIFWIND_NS {
#define NAMESPACE_NIFWIND }

#ifndef EOL
# ifdef _WIN32
#  define EOL "\r\n"
# else
#  define EOL "\n"
# endif
#endif

#define N_EXCEPTION 1
#define N_EXCEPTION_MSG "%s. Stop."

#include <cstdio>

NIFWIND_NAMESPACE
    class NException
    {
        public: NException(const char * f, int l) : src_{f}, row_{l} {}
        protected: NException(const char * name, const char * f, int l)
            : Name{name}, src_{f}, row_{l} {}

        public: const char * const Name{"Exception"};
        private: const char * const src_;
        private: int row_;
        public: inline virtual void Print() const
        {
            printf ("%s: %s:%d" EOL, Name, src_, row_);
        }
    };
    class NAssertionFailed final : public NException
    {
        public: NAssertionFailed(const char * f, int l)
            : NException {"AssertionFailed", f, l} {}
    };
NAMESPACE_NIFWIND

#ifdef NIFWIND_TESTING
# define NTHROW(E) throw E;
#else
# define NTHROW(E) { E.Print (); exit (N_EXCEPTION); }
#endif

#define NSURE(C,M) if (! (C)) \
    { printf (M EOL); NTHROW ((NAssertionFailed {__FILE__, __LINE__}))}

#endif