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

#ifndef _N_FFD_NODE_ADAPTER_H_
#define _N_FFD_NODE_ADAPTER_H_

#include "nifwind.h"

#include <QVector>  // the full power of "Qt" shall be used for now
#include <QVariant>

#include "ffd_node.h"
using FFDNode = FFD_NS::FFDNode;

NIFWIND_NAMESPACE

// Adapt FFDNode to the TreeModel.
// Deletes the "Nodes" pointers.
class NFFDNodeAdapter
{
    public: NFFDNodeAdapter * Base{};
    public: int Index; // Because QAbstractItemModel::parent()
    public: QVector<NFFDNodeAdapter *> Nodes {};
    public: inline int Count() { return Nodes.size (); }
    public: inline NFFDNodeAdapter * operator[](int i) { return Nodes[i]; }
    public: NFFDNodeAdapter(FFDNode * n = nullptr, NFFDNodeAdapter * b = nullptr)
        : n_{n}
    {
        if (b)
            Base = b, b->Nodes.push_back (this), Index = Nodes.size ()-1;
        // These are configured by the user: e.g. ensure the unnamed-yet method
        // reflects that.
        if (n_) {
            // TODO map to Options; diagram time: is adapter role to
            //      do such mapping? also, this shouldn't happen for each Node:
            //      there is no use case yet: where the user wants different
            //      set of fields shown per node?
            //      I'm really missing reflection here.
            // Otherwise this is exacly this adapter job: code like this should
            // happen here only: when FFDNode gets its interface modified,
            // only this file shall be updated.
            fields_.push_back (QString {n_->FieldNode ()->Name.AsZStr ()});
            fields_.push_back (QString {n_->FieldNode ()->TypeToString ()
                .AsZStr ()});
        }
        else {
            fields_.push_back ("Field 0");
            fields_.push_back ("Field 1");
        }
        // the node transforms to a tree
        if (n_ && ! n_->ArrayOfFields ())
            for (auto sn : n_->Nodes ())
                new NFFDNodeAdapter {sn, this};
    }
    public: ~NFFDNodeAdapter() { for (auto f : Nodes) delete f; }
    public: inline QVariant FieldById(int id)
    {
        return id >= 0 and id < fields_.size () ? fields_[id] : "MV";
    }
    // Why is this not pointless? Because it allows you to move columns about.
    private: QVector<QVariant> fields_;
    //TODO update _fields on set()
    private: FFDNode * n_{};
};// FFDNodeAdapter

NAMESPACE_NIFWIND
#endif