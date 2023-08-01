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

#ifndef _FFDNODEADAPTER_H_
#define _FFDNODEADAPTER_H_

//TODO should I?
#include <QVector>

#include <QVariant>

#include "ffd_node.h"
using FFDNode = FFD_NS::FFDNode;

namespace nifwind {

// Adapt FFDNode to the TreeModel.
// Deletes the "Nodes" pointers.
class FFDNodeAdapter
{
    public: FFDNodeAdapter * Base{};
    public: int Index; // Because QAbstractItemModel::parent()
    public: QVector<FFDNodeAdapter *> Nodes {};
    public: inline int Count() { return Nodes.size (); }
    public: inline FFDNodeAdapter * operator[](int i) { return Nodes[i]; }
    public: FFDNodeAdapter(FFDNode * n = nullptr, FFDNodeAdapter * b = nullptr)
        : _n{n}
    {
        if (b)
            Base = b, b->Nodes.push_back (this), Index = Nodes.size ()-1;
        // These are configured by the user: e.g. ensure the unnamed-yet method
        // reflects that.
        if (_n) {
            // TODO map to Options; diagram time: is adapter role to
            //      do such mapping? also, this shouldn't happen for each Node:
            //      there is no use case yet: where the user wants different
            //      set of fields shown per node?
            //      I'm really missing reflection here.
            // Otherwise this is exacly this adapter job: code like this should
            // happen here only: when FFDNode gets its interface modified,
            // only this file shall be updated.
            _fields.push_back (QString {_n->FieldNode ()->Name.AsZStr ()});
            _fields.push_back (QString {_n->FieldNode ()->TypeToString ()
                .AsZStr ()});
        }
        else {
            _fields.push_back ("Field 0");
            _fields.push_back ("Field 1");
        }
    }
    public: ~FFDNodeAdapter() { for (auto f : Nodes) delete f; }
    public: inline QVariant FieldById(int id)
    {
        return id >= 0 and id < _fields.size () ? _fields[id] : "MV";
    }
    // Why is this not pointless? Because it allows you to move columns about.
    private: QVector<QVariant> _fields;
    //TODO update _fields on set()
    private: FFDNode * _n{};
};// FFDNodeAdapter

}

#endif