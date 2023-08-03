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

#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QStyle>
#include <QDesktopWidget>
#include <QScreen>
#include <math.h>
#include "nifwind.h"

#include "n_main_window.h"

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
    QApplication app (argc, argv);
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