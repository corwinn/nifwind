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

#ifndef _N_MAIN_WINDOW_H_
#define _N_MAIN_WINDOW_H_

#include "nifwind.h"
#include "ffd.h"

#include <QMainWindow>

class QTreeView;

NIFWIND_NAMESPACE
class NHexView;

class NMainWindow final: public QMainWindow
{
    Q_OBJECT

    public: NMainWindow();
    public: ~NMainWindow() override;

    private slots: void HandleFileOpen();

    private: class FFDEntry final
    {
        public: FFDEntry(const FFDEntry & v) { operator= (v); }
        public: FFDEntry(FFDEntry && v)
        {
            operator= (static_cast<FFDEntry &&>(v));
        }
        public: FFDEntry & operator=(const FFDEntry & v)
        {
            //TODO make FFD copy-able
            return ffd_ = v.ffd_, (const_cast<FFDEntry &>(v)).ffd_ = nullptr,
                fn_ = v.fn_, *this;
        }
        public: FFDEntry & operator=(FFDEntry && v)
        {
            return ffd_ = v.ffd_, v.ffd_ = nullptr,
                fn_.operator= (static_cast<QString &&>(v.fn_)), *this;
        }
        public: FFDEntry(const QString &);
        // lets see if QList does auto~() ... it does
        public: ~FFDEntry() { if (ffd_) { delete ffd_; ffd_ = nullptr; } }
        private: FFD_NS::FFD * ffd_{};
        public: inline FFD_NS::FFD * FFD() { return ffd_; }
        private: QString fn_{}; //LATER remove if useless
    }; // FFDEntry
    private: QList<FFDEntry> ffd_ {};
    private: void InitFFD();

    private: QTreeView * tv_{}, * stv_{};
    private: NHexView * hv_{};
}; // NMainWindow

NAMESPACE_NIFWIND
#endif
