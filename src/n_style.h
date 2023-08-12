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

#ifndef _N_STYLE_H_
#define _N_STYLE_H_

#include "nifwind.h"

#include <QProxyStyle>
#include <QPainter>

NIFWIND_NAMESPACE

// Custom drawing goes here. I have no idea why would they put their draw procs
// in giant switch statements, but thats that.
// Usage: QApplication::setStyle (new NStyle);
class NStyle final : public QProxyStyle
{
    public: inline void drawPrimitive(PrimitiveElement pe,
        const QStyleOption * opt, QPainter * p, const QWidget * w = nullptr)
        const override
    {
        // Q_D(const NStyle); // related to the Q_DECLARE_PRIVATE below

        if (QStyle::PE_IndicatorBranch == pe) {
            QProxyStyle::drawPrimitive (pe, opt, p, w);
            //TODO render the usual tree + trace path
            p->setPen (QColor {1,1,1}); // clip/bound rectangle
            p->drawRect (opt->rect);
            int mx = opt->rect.center ().x (), my = opt->rect.center ().y ();
            if (opt->state & State_Item) {
                // Some of them are inverted?!
                // |    |    |    |                   |    |
                // |  --| vs |--  |, really puzzling: |----|
                // |    |    |    |                   |    |
                p->setPen (QColor {222,1,1});
                p->drawLine (mx, my, opt->rect.width (), my);
                p->drawLine (mx, my, opt->rect.width (), opt->rect.y ());
            }

        }
        else
            QProxyStyle::drawPrimitive (pe, opt, p, w);
    }
    public: bool TreeTracePath{};
    // private: Q_DECLARE_PRIVATE(NStyle)
};

NAMESPACE_NIFWIND

#endif
