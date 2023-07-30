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

#ifndef _TREEMODEL_H_
#define _TREEMODEL_H_

#include <QAbstractItemModel>

namespace nifwind {

// Courtesy of "qtbase/src/widgets/dialogs/qfilesystemmodel.*".
// For the "QModelIndex" mappings, see "TreeModel.dia".
//
// NodeAdapter - adapts FFDNode to "QModelIndex" & co:
//
//   * maps FFDNode fields to columns on the view; since I'm allowing
//     re-grouping of the tree:
//       - default: and you get the "nif" block tree
//       - group by type *: and you get the "nif" by-block-type sub-trees
//       - group by ref/ptr: and you get the "usual" "nif" "tree"
//       - group by type 1:  and you get 1 type only tree
//       for starters
//       example:
//           0 1                1 0
//         r                  r
//         +-a    group-by 1  +-b
//           +-b     ->       | +-a
//           +-c              +-c
//                              +-a
//       How: 1. un-group the tree to a table; 2. group by the new column:
//           0 1
//           a b
//           a c
//
// NodeAdapter { // so far...
//    NodeAdapter * Base
//    int Index                       // the index of this at Base[]
//    NodeAdapter * Nodes
//    int Count()                     // number of sub-nodes at Nodes
//    NodeAdapter * operator[](int i) // [] access at Nodes
//    QVariant FieldById(column)
// }
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

    // mandatory ---------------------------------------------------------------

    // Bind qmi to tree-node; see TreeModel.dia
    private: QModelIndex index(int r, int c,
        const QModelIndex & n = QModelIndex {}) const override
    {
        printf ("m: index(%d, %d, (%d, %d))\n", r, c, n.row (), n.column ());
        // "qtbase/src/corelib/itemmodels/qabstractitemmodel.cpp":
        // range check: r in [0;row.count) && c in [0;col.count)
        if (! QAbstractItemModel::hasIndex (r, c, n)) return QModelIndex {};

        auto sub_node = this->Index2Node (n)->operator[] (r);
        return sub_node ? createIndex (r, c, sub_node) : QModelIndex {};
    }
    private: QModelIndex parent(const QModelIndex & n)
        const override
    {
        printf ("m: parent((%d, %d))\n", n.row (), n.column ());
        if (! n.isValid ()) return QModelIndex {};
        auto node = static_cast<NodeAdapter *>(n.internalPointer ());
        if (node->Base == _tree) return QModelIndex {};
        // This is quite peculiar requirement - e.g. the adapter
        return createIndex (node->Base->Index, 0, node->Base);
    }
    private: int rowCount(const QModelIndex & n = QModelIndex {})
        const override
    {
        printf ("m: rowCount((%d, %d))\n", n.row (), n.column ());
        if (n.column () > 0) return 0; // see TreeModel.dia
        return this->Index2Node (n)->Count ();
    }
    private: int columnCount(const QModelIndex & n = QModelIndex {})
        const override
    {
        printf ("m: columnCount((%d, %d))\n", n.row (), n.column ());
        return 5;
    }
    private: QVariant data(const QModelIndex & n, int r = Qt::DisplayRole)
        const override
    {
        printf ("m: data((%d, %d), %d)\n", n.row (), n.column (), r);
        if (! n.isValid ()) return QVariant {};
        if (r != Qt::DisplayRole) return QVariant {};

        auto node = static_cast<NodeAdapter *>(n.internalPointer ());
        return node->FieldById (n.column ());
    }

    // didn't help showing my single simple node
    private: Qt::ItemFlags flags(const QModelIndex & n) const override
    {
        if (! n.isValid ()) return Qt::NoItemFlags;
        return QAbstractItemModel::flags (n);
    }
    private: QVariant headerData(int s, Qt::Orientation o, int r) const override
    {
        if (Qt::Horizontal == o && Qt::DisplayRole == r)
            return s;
        return QVariant {};
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