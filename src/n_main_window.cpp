//  "The Last Hunt begins, Young Bull! Hopper screamed. We live. We live!"

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

#include "n_main_window.h"
#include "n_options.h"

#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QFileDialog>
#include <QTreeView>
#include <QDockWidget>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QStringList>

#include "n_tree_model.h"
#include "n_ffd_node_adapter.h"
#include "n_file_stream.h"
#include "n_hex_view.h"
#include "n_tree_view.h"

NIFWIND_NAMESPACE

NMainWindow::NMainWindow()
    : QMainWindow {}
{
    InitFFD (); // load all available FFD

    // main menu
    auto miFile = menuBar ()->addMenu ("&File");
        auto aOpen =
            miFile->addAction ("&Open", this, &NMainWindow::HandleFileOpen);
        aOpen->setEnabled (! ffd_.empty ()); //TODO update via timer
        miFile->addSeparator ();

        auto aQuit = miFile->addAction ("&Quit");
        connect (aQuit, &QAction::triggered, this, &NMainWindow::close);

    auto miHelp = menuBar ()->addMenu ("&Help");
        miHelp->addAction ("&About Qt", qApp, &QApplication::aboutQt);

    /*// TODO to a unit test:
    int arr[] = {1, 2, 3};
    NOptions::PutIntArr ("foo", QVector<int> {arr, arr+3});
    auto arr2 = NOptions::GetIntArr ("foo");
    qDebug () << arr2;
    printf ("arr %d: %d, %d, %d" EOL, arr2.size (), arr2[0], arr2[1], arr2[2]);
    */

    tv_ = new NTreeView {};
    auto foo = new QDockWidget {"TreeView"};
    foo->setWidget (tv_);
    addDockWidget (Qt::LeftDockWidgetArea, foo);

    hv_ = new NHexView {};
    foo = new QDockWidget {"HexView"};
    foo->setWidget (hv_);
    addDockWidget (Qt::RightDockWidgetArea, foo);

    statusBar ()->showMessage ("Idle");
} // NMainWindow::NMainWindow()

NMainWindow::~NMainWindow() {}

void NMainWindow::InitFFD()
{
    //TODO to options (ffd_files = avoid QDirIterator ... (NIFWIND_FFD_DIR))
    //     avoid using anything IO-related that operates with QString;
    //     (QDirIterator fails miserably - just confirmed by a test at main())
    //     use the OS API directly - see n_file_stream for details
    NMainWindow::FFDEntry one {"nif_ffd"};
    ffd_ << one;
}

//TODO remove QString from here; replace with "const char *" - see n_file_stream
//     for details
NMainWindow::FFDEntry::FFDEntry(const QString & fn)
    : fn_ {fn}
{
    //TODO do something about this: like ffd.FromStream ()
    NFileStream ffd_stream {fn.toUtf8 ().constData ()};
    FFD_NS::ByteArray ffd_buf {};
    qDebug () << "Using \"" << fn << "\" (" << ffd_stream.Size ()
        << " bytes) FFD";
    //TODO FFD_MAX_SIZE
    NSURE(ffd_stream.Size () > 0 && ffd_stream.Size () < 1<<20,
        "Suspicious FFD size")
    ffd_buf.Resize (ffd_stream.Size ());
    ffd_stream.Read (ffd_buf.operator byte * (), ffd_stream.Size ());
    Dbg.Enabled = false; //TODO un-mix my Dbg and QDebug()
    ffd_ = new FFD_NS::FFD {ffd_buf.operator byte * (), ffd_buf.Length ()};
}

void NMainWindow::HandleFileOpen()
{
    NSURE(! ffd_.empty (), "bug: Open File shall be disabled when no FFDs")

    //TODO I'm not sure what has to be done here - this won't show you non-
    //     unicode file paths at all; no error, no warning - you just won't see
    //     them; perhaps NFileDialog is the best option: NTableView +
    //     NFileSystemModel
    //TODO https://bugreports.qt.io/
    QFileDialog dlg {this};
    dlg.setFileMode (QFileDialog::ExistingFile);
    dlg.setNameFilter ("Nif files (*.nif *.nifcache *.texcache)");
    dlg.setViewMode (QFileDialog::Detail);
    if (! dlg.exec ()) return;

    NSURE(1 == dlg.selectedFiles ().size (), "unexpected number of files")
    // NTextCodec: .toLocal8Bit ().constData ()
    NFileStream data_stream {dlg.selectedFiles ()[0]};
    qDebug () << "Parsing " << dlg.selectedFiles ()[0] << ", "
        << data_stream.Size () << " bytes";
    //TODO loop over ffd_ - tryparse()
    //TODO progress bar
    FFD_NS::FFDNode * tree = ffd_[0].FFD ()->File2Tree (data_stream);
    NSURE(nullptr != tree, "parse_nif(): File2Tree() returned null?!")
    // tree->PrintTree ();
    auto root = new NFFDNodeAdapter {tree}; // can't be shown
    auto tree_model = new NTreeModel<NFFDNodeAdapter> (root, this);
    tv_->setModel (tree_model);
}

NAMESPACE_NIFWIND
