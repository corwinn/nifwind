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

#ifndef _N_TEXT_CODEC_H_
#define _N_TEXT_CODEC_H_

#include "nifwind.h"
#include <QTextCodec>

NIFWIND_NAMESPACE

// Solution 1: To the Bridge.
//
// "Qt", you're not the people that will dictate the encoding of a File
// Path/Name; - That gets decided by the File System authors and its users.
// When I pass you a "const char *" - you touch it not; it is as simple as that.
// Wherever I'll interpret it as some encoding is up to me! - do you get that? -
// or shall I render it to you?!
//LATER - a 3D rendering of "do not decide my encoding instead of me"
//
// This code exists to solve a problem that shouldn't exist.
class NTextCodec final : public QTextCodec
{
    public: inline QByteArray name() const override
    {
        static QByteArray n {"DoNotTouchTC"};
        return n;
    }
    public: inline int mibEnum() const override { return -10000; }
    // Courtesy of: qtbase/src/corelib/codecs/qisciicodec.cpp
    // Packing these tightly results in an application that is unable to start:
    //   "__cxa_guard_acquire detected recursive initialization"
    // e.g. the loops.
    public: inline QByteArray convertFromUnicode(const QChar * uc, int len,
        QTextCodec::ConverterState * cs) const override
    {
        static auto UTF8 = QTextCodec::codecForName ("UTF-8");
        // courtesy of QTextCodec::canEncode()
        /*QTextCodec::ConverterState state {};
        state.flags = QTextCodec::ConversionFlag::ConvertInvalidToNull;
        UTF8->fromUnicode (uc, len, &state);
        // printf ("convertFromUnicode.invalidChars: %d" EOL, state.invalidChars);
        if (0 == state.invalidChars)
            return UTF8->fromUnicode (uc, len, cs);*/

        QByteArray result {len, Qt::Uninitialized};
        uchar * ch = reinterpret_cast<uchar *>(result.data ());
        for (int i = 0; i < len; i++)
            *ch++ = static_cast<uchar>(uc[i].unicode ());

        // courtesy of QTextCodec::canEncode()
        QTextCodec::ConverterState state {};
        state.flags = QTextCodec::ConversionFlag::ConvertInvalidToNull;
        UTF8->toUnicode (reinterpret_cast<char *>(result.data ()), len, &state);
        // printf ("convertFromUnicode.invalidChars: %d" EOL, state.invalidChars);
        if (0 == state.invalidChars)
            return UTF8->fromUnicode (uc, len, cs);

        return result;
    }
    QString convertToUnicode(const char * ch, int len,
        QTextCodec::ConverterState * cs) const override
    {
        // printf ("convertToUnicode(%s)" EOL, ch);
        static auto UTF8 = QTextCodec::codecForName ("UTF-8");
        // courtesy of QTextCodec::canEncode()
        QTextCodec::ConverterState state {};
        state.flags = QTextCodec::ConversionFlag::ConvertInvalidToNull;
        UTF8->toUnicode (ch, len, &state);
        // printf ("convertToUnicode.invalidChars: %d" EOL, state.invalidChars);
        if (0 == state.invalidChars)
            return UTF8->toUnicode (ch, len, cs);

        QString result {len, Qt::Uninitialized};
        QChar * uc = result.data ();
        for (int i = 0; i < len; i++)
            *uc++ = static_cast<uchar>(ch[i]);
        return result;
    }
}; // NTextCodec

NAMESPACE_NIFWIND

#endif

/*
    // test a file path containing non-unicode path names:
    // confirmed: it doesn't list the non-unicode entry at all
    //TODONT https://bugreports.qt.io/
    //     1. Make a directory:
    //        > mkdir a$'\xe9'b
    //     1. Make a file:
    //        > echo what the nice mountain view > c$'\xe9'd
    //     2. let your QDirIterator list it
    //     2. let File::exists () find it
    //     2. let your QFile open it
    //     2. let your QFileDialog show it
    // Why would you ... I don't know how to define the above ... it?
    // This ^ makes your FILE IO completely unreliable Mr. "rare enough to not
    // be worth"; - Why is this level of "reasoning" even allowed at "Qt"?!
    //
    // The ... calls for a bridge; I can not guess what else these people have
    // "decided" to "force" for no apparent reason.
    if (2 == argc) {
        qDebug () << "Iterating" << argv[1];
        QDirIterator foo {argv[1]};
        while (foo.hasNext ())
            qDebug () << foo.next ();
        qDebug () << "Thats not all, unfortunately";
    }
 */
