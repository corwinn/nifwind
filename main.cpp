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

//c make
//r LSAN_OPTIONS=suppressions="${PWD}/asan_supp" ./nifwind

struct QUnreliable {};

//TODO re-design; decouple as much as possible from "Qt" because its unreliable.

#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QStyle>
#include <QDesktopWidget>
#include <QScreen>
#include <QDebug>
#include <QDirIterator>
#include <math.h>
#include "nifwind.h"

#include "n_main_window.h"
#include "n_text_codec.h"

class foo final : public QOpenGLWidget, protected QOpenGLFunctions
{
    public: using QOpenGLWidget::QOpenGLWidget;
    private: void initializeGL() override
    {
        initializeOpenGLFunctions ();
    }
    private: inline void resizeGL(int w, int h) override
    {
        glViewport (0, 0, static_cast<GLint>(w), static_cast<GLint>(h));
    }
    private: float _t {};
    private: inline void paintGL() override
    {
        glClearColor (sinf (_t) * .5f + .5f, cosf (_t) * .5f + .5f,
            sinf (6.28f - _t) * .5f + .5f, 1.0f);
        _t += 0.01f;
        if (_t > 6.28f) _t = float {};
        glClear (GL_COLOR_BUFFER_BIT);

        void (foo::*post_redisplay)() = &foo::update;
        QTimer::singleShot (1000/60, this, post_redisplay);
    }
};// QOpenGLWidget

int main(int argc, char **argv)
{
    // This is a temporary solution. The moment they break this, the "Qt" File
    // IO related code shall become unreliable again.
    (void)new NIFWIND_NS::NTextCodec; // a.k.a. register()
    QTextCodec::setCodecForLocale (QTextCodec::codecForName ("DoNotTouchTC"));

    QApplication app (argc, argv);

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

    nifwind::NMainWindow mw {};
    foo bar {};
    mw.setCentralWidget (&bar);
    mw.resize (800, 600);
    mw.setWindowTitle (NIFWIND_PROJECT);
    mw.setGeometry (
        QStyle::alignedRect (
            Qt::LeftToRight,
            Qt::AlignCenter,
            mw.size (),
            app.screens ().first ()->geometry ()));
    mw.show ();
    return app.exec ();
}