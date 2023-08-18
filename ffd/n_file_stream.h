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

#ifndef _N_FILE_STREAM_H_
#define _N_FILE_STREAM_H_

#include "nifwind.h"

#include "ffd_model.h"

#include <QDebug>
#include <QString>
#include <QFile>

NIFWIND_NAMESPACE

//LATER this is unreliable due to QString rendering arbitrary files
//      non-existent. Delete it. QString can not override the FS.
//      For now NTextCodec solves the artificial issue.
// Read-only, for now.
class NFileStream final : public ::FFD_NS::Stream
{
    public: Stream & Read(void * buf, size_t n = 1) override // bytes
    {
        NSURE(f_.read (reinterpret_cast<char *>(buf), n)
            == static_cast<qint64>(n), "read() failed")
        return *this;
    }
    public: off_t Tell() const override { return f_.pos (); }
    // No idea what this does, so be careful with its calls per second
    public: off_t Size() const override { return f_.size (); }
    public: Stream & Seek(off_t o) override // relative - always
    {
        NSURE(f_.seek (f_.pos () + o), "seek() failed")
        return *this;
    }
    public: Stream & Reset() override // you've just been constructed
    {
        // There is a warning about this and text files, so be alert.
        // Why would you create methods that do work sometimes, as expected?!
        NSURE(f_.reset (), "reset() failed")
        return *this;
    }
    public: NFileStream(const QString & n) : Stream {}, f_ {n}
    {
        if (! QFile::exists (n))
            qDebug () << "File does not exist:" << n;
        NSURE(f_.open (QIODevice::ReadOnly), "Can't open file")
    }
    public: ~NFileStream() override {}
    private: QFile f_;
};// NFileStream

NAMESPACE_NIFWIND

#endif
