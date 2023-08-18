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

#ifndef _N_SNODE_ADAPTER_H_
#define _N_SNODE_ADAPTER_H_

#include "nifwind.h"

#include <QVector>
#include <QVariant>

#include "ffd.h"
using SNode = FFD_NS::FFD::SNode;

NIFWIND_NAMESPACE

// Adapt FFD::SNode to the TreeModel.
// Deletes the "Nodes" pointers.
class NSNodeAdapter
{
    public: NSNodeAdapter * Base{};
    public: int Index; // Because QAbstractItemModel::parent()
    public: QVector<NSNodeAdapter *> Nodes {};
    public: inline int Count() { return Nodes.size (); }
    public: inline NSNodeAdapter * operator[](int i) { return Nodes[i]; }
    public: NSNodeAdapter(SNode * n = nullptr, NSNodeAdapter * b = nullptr)
        : n_{n}
    {
        if (b)
            Base = b, b->Nodes.push_back (this), Index = b->Nodes.size ()-1;
        // These are configured by the user: e.g. ensure the unnamed-yet method
        // reflects that.
        if (n_) {
            // TODO see the NFFDNodeAdapter TODO
            // When SNode gets its interface modified, only this file shall be
            // updated.
            fields_.push_back (QString {n_->Name.AsZStr ()});
            if (n_->DType)
                fields_.push_back (QString {n_->DType->Name.AsZStr ()});
            else fields_.push_back ("null");
            fields_.push_back (QString {n_->TypeToString ().AsZStr ()});
        }
        else {
            fields_.push_back ("Field 0");
            fields_.push_back ("Field 1");
            fields_.push_back ("Field 3");
        }
        // the node transforms to a tree
        if (! n_) return;
        if (! Base)
            n_->WalkForward ([&](SNode * n) -> bool {
                if (nullptr != n && n_ != n) new NSNodeAdapter {n, this};
                return true;
            });
        else for (auto sn : n_->Fields)
            if (sn) new NSNodeAdapter {sn, this};
    }
    public: ~NSNodeAdapter()
    {
        for (auto f : Nodes) delete f;
    }
    public: inline QVariant FieldById(int id)
    {
        return id >= 0 and id < fields_.size () ? fields_[id] : "MV";
    }
    // Why is this not pointless? Because it allows you to move columns about.
    private: QVector<QVariant> fields_;
    //TODO update _fields on set()
    private: SNode * n_{};
};// NSNodeAdapter

NAMESPACE_NIFWIND
#endif
