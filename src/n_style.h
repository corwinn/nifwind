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

// layout helper: center "c" againt "b", at offset "a"
auto center = [](int a, int b, int c) { return a + (b-c) / 2; };

// Custom drawing goes here. I have no idea why would they put their draw procs
// in giant switch statements, but thats that.
// Usage: QApplication::setStyle (new NStyle);
class NStyle final : public QProxyStyle
{
    public: NStyle()
        : QProxyStyle {}
    {
        tree_pen_node_dash_.setStyle (Qt::DotLine);
    }

    public: inline void drawPrimitive(PrimitiveElement pe,
        const QStyleOption * opt, QPainter * p, const QWidget * w = nullptr)
        const override
    {
        // Q_D(const NStyle); // related to the Q_DECLARE_PRIVATE below

        if (QStyle::PE_IndicatorBranch == pe) {
            //TODO render non-draft tree + trace path
            int mx = opt->rect.center ().x (), my = opt->rect.center ().y (),
                l = opt->rect.x (), r = l + opt->rect.width (),
                t = opt->rect.y (), b = t + opt->rect.height ();
            p->setPen (tree_pen_tree_);
            if (opt->state & QStyle::StateFlag::State_Sibling) {
                p->drawLine (mx, t, mx, b);
                if (opt->state & QStyle::StateFlag::State_Children
                    || opt->state & QStyle::StateFlag::State_Item)
                    p->drawLine (mx, my, r, my);
            }
            else {
                if (opt->state & QStyle::StateFlag::State_Item) {
                    p->drawLine (mx, t, mx, my);
                    p->drawLine (mx, my, r, my);
                }
            }
            if (opt->state & QStyle::StateFlag::State_Children) {
                p->setPen (tree_pen_node_dash_);
                int a = opt->rect.width () > opt->rect.height ()
                    ? opt->rect.height ()
                    : opt->rect.width ();
                a -= 4;
                int l1 = center (l, opt->rect.width (), a),
                    t1 = center (t, opt->rect.height (), a),
                    r1 = l1+a-1, b1 = t1+a-1;
                p->fillRect (l1, t1, a, a, tree_brush_node_);
                p->drawLine (l1, t1, l1, b1);
                p->drawLine (l1, b1, r1, b1);
                p->drawLine (r1, b1, r1, t1);
                p->drawLine (r1, t1, l1, t1);
                // p->drawRect (l1, t1, a-1, a-1);
                p->setPen (tree_pen_node_);
                p->drawLine (mx-2, my, mx + 2, my);
                if (! (opt->state & QStyle::StateFlag::State_Open))
                    p->drawLine (mx, my-2, mx, my + 2);
            }
        }
        else
            QProxyStyle::drawPrimitive (pe, opt, p, w);
    }
    public: bool TreeTracePath{};
    private: QPen tree_pen_tree_ {QColor {171, 171, 171}};
    private: QPen tree_pen_node_ {QColor {11, 11, 11}};
    private: QPen tree_pen_node_dash_ {QColor {11, 11, 11}};
    private: QBrush tree_brush_node_ {QColor {234, 234, 234}};
    // private: Q_DECLARE_PRIVATE(NStyle)
};

NAMESPACE_NIFWIND

#endif
