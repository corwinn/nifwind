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

/*
  legend (context: trees, their views, models, adapters, delegates, factories):

    r   - row
    c   - column
    n   - node
    qmi - QModelIndex
*/

#ifndef _N_HEX_VIEW_MODEL_H_
#define _N_HEX_VIEW_MODEL_H_

#include "nifwind.h"

#include <QAbstractItemModel>
#include <QBrush>
#include <QIODevice>

NIFWIND_NAMESPACE

// Show file binary data as a table NxM where each cell renders one byte of the
// file data with this format: %002X
// The catch: render exactly the needed amount of data at a time: yes you can
// view terabyte-sized file - thats the whole idea.
class NHexViewModel final : public QAbstractTableModel
{
    public: explicit NHexViewModel(QIODevice * n = nullptr,
        QObject * b = nullptr)
        : QAbstractTableModel {b}, s_{n}
    {
    }
    public: virtual ~NHexViewModel() override {}

    // mandatory ---------------------------------------------------------------

    private: inline int rowCount(const QModelIndex & = QModelIndex {})
        const override
    {
        return 1<<24;
    }

    private: inline int columnCount(const QModelIndex & = QModelIndex {})
        const override
    {
        return 16;
    }

    private: inline QVariant data(const QModelIndex & n, int r = Qt::DisplayRole)
        const override
    {
        if (! n.isValid ()) return QVariant {};
        if (Qt::TextAlignmentRole == r)
            return Qt::AlignBottom;
        if (r != Qt::DisplayRole)
            return QVariant {};

        // I love the output of this statement: well nice mountain view done Qt.
        // printf ("N: %d, M: %d" EOL, n.row (), n.column ());

        return QString {"%1"}
            .arg (0xff & ((n.row () << 4) + n.column ()), 2, 16, QChar{'0'})
            .toUpper ();
    }

    private: inline QVariant headerData(int s, Qt::Orientation o, int r)
        const override
    {
        if (Qt::Horizontal == o && Qt::DisplayRole == r)
            return s;
        return QVariant {};
    }

    private: QIODevice * s_{};
};// NHexViewModel

NAMESPACE_NIFWIND
#endif