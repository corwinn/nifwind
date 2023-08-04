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

#if defined(_FFD_MODEL_STRING_H_)
#error This file has to be included at 1 place only
#endif

#ifndef _FFD_MODEL_STRING_H_
#define _FFD_MODEL_STRING_H_

// X: List<String>
#define FFD_STRING_IMPL(X) MyString<X>

#include <QString>
#include <QStringList>
#include <QByteArray>

template <typename T> class MyString final
{
    public: MyString() : _p {} {}
    public: explicit MyString(const byte * b, int l)
        : _p {QByteArray {reinterpret_cast<const char *>(b), l}} {}
    public: MyString(const char * c) : _p {c} {}
    public: MyString(const MyString & s) { operator= (s); }
    public: MyString(MyString && s) { operator= (static_cast<MyString &&>(s)); }
    public: MyString & operator=(const MyString & v)
    {
        return _p = v._p, *this;
    }
    public: MyString & operator=(MyString && v)
    {
        return _p.operator= (static_cast<QString &&>(v._p)), *this;
    }
    public: bool Empty() const { return _p.isEmpty (); }
    public: bool operator==(const MyString & v) const { return _p == v._p; }
    public: bool operator!=(const MyString & v) const { return _p != v._p; }
    public: bool operator==(const char * b) const
    {
        auto a = AsZStr ();
        if (nullptr == a && nullptr == b) return true;//TODO should it?
        if (nullptr == a || nullptr == b) return false;
        while (*a || *b)
            if (*a++ != *b++) return false;
        return true;
    }
    public: const char * AsZStr() const
    {
        // nope; whatever this is it ain't UTF-8:
        //  * return reinterpret_cast<const char *>(_p.unicode ());
        //  * return reinterpret_cast<const char *>(_p.constData ());

        // why make it simple when you can make it complex
        static QByteArray _8 {};
        return (_8 = QByteArray {_p.toUtf8 ()}).constData ();
    }
    public: int Length() const { return static_cast<int>(_p.size ()); }
    public: T Split(char d) // List<String>
    {
        T r {};
        // their .split (d) returns 1 token on empty string
        if (this->Empty ()) return r;
        auto l = _p.split (d);
        for (auto & t : l) r.Add (t.toUtf8 ().constData ());
        return r;
    }
    private: QString _p;
};

#endif
