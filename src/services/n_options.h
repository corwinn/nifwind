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

#ifndef _N_OPTIONS_H_
#define _N_OPTIONS_H_

#include "nifwind.h"
#include <QVector>
#include <QMetaType>

NIFWIND_NAMESPACE

// You can put things here. And you can get them. You could even get notified
// when things you put here got changed - neat.
// Things you put here could get persisted.
//
// This is a key-value store: means you have to be careful with the keys: a
// duplicate one will reward you with an exception (update = false). Recommended
// usage:
//   object_name.domain.sub-domain.data_name
//   example: "mainform.dockwidgets.treeview.x"
// A non existent one will reward you with an exception as well.
//
// You can use Q_DECLARE_METATYPE as well, because this is
// QMap<QString, QVariant>.
//
// Type support shall be added on an as-needed basis, via the macros:
//   - n_options.h  : N_OPT      (type alias, actual type)
//   - n_options.cpp: N_PUT_GET  (type alias)
//                    N_PUT_GET_M(type alias) - for Q_DECLARE_METATYPE
//   - Q_DECLARE_METATYPE - add to the list below
//  (if you get an error with N_PUT_GET you should use N_PUT_GET_M)
//  (the type alias is a Get/Put suffix as well - so it must comply with C++)
// Given a type alias "Foo", you can:
//   - PutFoo(key, value) - to put a new non-existent key-value
//   - PutFoo(key, value, NOptions::Put::Update) - to update an existing value
//   - GetFoo(key) - to get a previously stored value
//
// Thread-unsafe. This shall be used by the single main thread only. Its
// persistence shall be made via a copy created by the main thread and queued
// at the async. IO (LATER MemoryStream AsStream(); FromStream(Stream &)).
//
// Please do not store 10000-node-trees here; Please do not query this 60-times
// per second.
class NOptions final // program options
{
    N_NO_COPY(NOptions)
    N_NO_MOVE(NOptions)
    private: NOptions() {}

    enum class Put {New, Update}; // because: no named parameters
#define N_OPT(T,R) \
    using T = R; \
    public: static void Put##T(const char * k, T v, Put op = Put::New); \
    public: static T Get##T(const char * k);

    N_OPT(Int, int)
    N_OPT(IntArr, QVector<int>)

#undef N_OPT
};

NAMESPACE_NIFWIND

Q_DECLARE_METATYPE(QVector<int>)

#endif
