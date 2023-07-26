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

#ifndef _TREEMODEL_H_
#define _TREEMODEL_H_

#include <QAbstractItemModel>

namespace nifwind {

// Courtesy of "qtbase/src/widgets/dialogs/qfilesystemmodel.*".
//TODO Why wouldn't I use a template for a Model?
//
// "A valid index belongs to a model, and has non-negative row and column
// numbers."
// What does "hierarchy of tables" mean?! Hierarchy of their top-left corners?!
// "If you do not make use of the hierarchy, then the model is a simple table of
// rows and columns"
template <typename NodeAdapter> class TreeModel final
    : public QAbstractItemModel
{
    public: explicit TreeModel(NodeAdapter * n = nullptr, QObject * b = nullptr)
        : QAbstractItemModel {b}, _tree{n}
    {
    }
    public: virtual ~TreeModel() override
    {
        if (_tree) delete _tree; // temporary here
    }

    // mandatory
    private: QModelIndex index(int r, int c, const QModelIndex & n = QModelIndex{})
        const override
    {
        if (! QAbstractItemModel::hasIndex (r, c, n)) return QModelIndex {};
        auto sub_node = Index2Node (n)->operator[] (r);
        return sub_node ? createIndex (r, c, sub_node) : QModelIndex {};
    }
    private: QModelIndex parent(const QModelIndex & n)
        const override
    {
        if (! n.isValid ()) return QModelIndex {};
        auto node = static_cast<NodeAdapter *>(n.internalPointer ());
        if (node->Base == _tree) return QModelIndex {};
        // This is quite peculiar requirement - e.g. the adapter
        return createIndex (node->Base->Index, 0, node->Base);
    }
    private: int rowCount(const QModelIndex & n = QModelIndex{})
        const override
    {
        if (n.column () > 0) return 0; // Because?! Qt, could you please document ...
        return Index2Node (n)->Count ();
    }
    private: int columnCount(const QModelIndex & = QModelIndex{})
        const override { return 5; }
    private: QVariant data(const QModelIndex & n, int r = Qt::DisplayRole)
        const override
    {
        if (! n.isValid ()) return QVariant {};
        if (r != Qt::DisplayRole) return QVariant {};

        auto node = static_cast<NodeAdapter *>(n.internalPointer ());
        // map index.column () to node fields
        switch (n.column ()) {
            case 0: return node->Name;
            default: return "MV";
        }
    }

    private: NodeAdapter * Index2Node(const QModelIndex & i) const
    {
        return ! i.isValid () ? _tree
            : static_cast<NodeAdapter *>(i.internalPointer ());
    }
    private: NodeAdapter * _tree{};
};// TreeModel

}

#endif