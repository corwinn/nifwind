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

#if defined(_FFD_MODEL_LIST_H_)
#error This file has to be included at 1 place only
#endif

#ifndef _FFD_MODEL_LIST_H_
#define _FFD_MODEL_LIST_H_

#define FFD_LIST_IMPL MyList<T>

#include <QVector>

// cleanup - see ~MyList() below
template <typename T> struct LD { void operator()(T & t) { t.~T (); } };
template <typename T> struct LD<T *> { void operator()(T *&) {} };

template <typename T> class MyList final
{
    public: inline MyList() {}
    public: inline ~MyList()
    {
        //LATER check QVector ~() capabilities
        // for (int i = 0; i < Count (); i++) LD<T>{} (_p[i]);
    }
    public: inline MyList(const MyList<T> & v) { operator= (v); }
    public: inline MyList(MyList<T> && v) { operator= (v); }
    public: inline const T * begin() const
    {
        return _p.begin () != _p.end () ? &(*_p.begin ()) : nullptr;
    }
    public: inline const T * end  () const
    {
        return _p.end () != _p.begin () ? &(*_p.end ()) : nullptr;
    }
    public: inline int Count() const { return _p.size (); }
    public: inline T & Add(const T & v) { return _p.push_back (v), _p.back (); }
    public: inline MyList<T> & Put(T && v)
    {
        return _p.push_back (static_cast<T &&>(v)), *this;
    }
    public: inline T & operator[](int i) { return _p[i]; }
    public: inline const T & operator[](int i) const { return _p[i]; }
    public: inline MyList<T> & operator=(const MyList<T> & v)
    {
        return _p.operator= (v._p), *this;
    }
    public: inline MyList<T> & operator=(MyList<T> && v)
    {
        return _p.operator= (static_cast<QVector<T> &&>(v._p)), *this;
    }
    public: template <typename Fd> inline T * Find(Fd on_itm)
    {
        for (int i = 0; i < _p.size (); i++)
            if (on_itm (static_cast<const T &>(_p[i])))
                return &(_p[i]);
        return nullptr;
    }
    private: QVector<T> _p {};
};

#endif
