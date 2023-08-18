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

#include "n_hex_view.h"
#include "n_hex_view_model.h"

#include <QFont>
#include <QFontInfo>
// #include <QFontDatabase>
#include <QHeaderView>
#include <QDebug>

NIFWIND_NAMESPACE

NHexView::NHexView(QWidget * base)
    : QTableView::QTableView {base}, cleanup1_ {new NHexViewModel}
{
    this->setSortingEnabled (false);
    this->setWordWrap (false);
    this->horizontalHeader ()->setHighlightSections (false);
    this->horizontalHeader ()->setSectionsClickable (false);
    this->horizontalHeader ()->setSectionsMovable (false);
    // try setting up mono-space font: this is required for this view
    auto hvf = QFont {this->font ()};
    hvf.setStyleHint (QFont::Monospace);
    QFontInfo hvfi {hvf};
    qDebug () << hvfi.family () << hvfi.fixedPitch ();
    if (! hvfi.fixedPitch ()) {
        hvf = QFont {"Liberation mono"};
        hvf.setStyleHint (QFont::Monospace);
        hvfi = QFontInfo {hvf};
        qDebug () << hvfi.family () << hvfi.fixedPitch ();
    }
    this->setFont (hvf);
    // qDebug () << QFontDatabase {}.families ();
    // stay fixed: all rows an all columns
    //TODO font_changed()
    this->horizontalHeader ()->setSectionResizeMode (QHeaderView::Fixed);
    this->verticalHeader ()->setSectionResizeMode (QHeaderView::Fixed);
    QFontMetricsF out_of_names {hvf};
    QRectF br {};
    for (int i = 0; i < 256; i++) {
        // Never works as expected - ever. Is it so hard to compute a nice
        // mountain view rectangle?
        //TODO add these options for the user:
        //      * font
        //      * hex view cell size / or padding/margins
        //        (to be used when auto-size presents them with a nice empty
        //         grid of quads)
        auto r1 = out_of_names.boundingRect (
            QString {"%1"}.arg (i, 2, 16, QChar{'0'}));
        if (r1.width () > br.width ()) br.setWidth (r1.width ());
        if (r1.height () > br.height ()) br.setHeight (r1.height ());
    }
    auto ttw = static_cast<int>(round (br.width () + 1.0));
    auto tth = static_cast<int>(round (br.height () + 1.0));
    ttw += ttw & 1; tth += tth & 1;
    // hv_->resizeRowsToContents (); // unfortunately this isn't ok
    this->horizontalHeader ()->setMinimumSectionSize (ttw);
    this->horizontalHeader ()->setDefaultSectionSize (ttw);
    this->verticalHeader ()->setMinimumSectionSize (tth);
    this->verticalHeader ()->setDefaultSectionSize (tth);

    this->setModel (cleanup1_.data ());
}// NHexView::NHexView()

NHexView::~NHexView() {}

NAMESPACE_NIFWIND
