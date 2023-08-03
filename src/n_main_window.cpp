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

#include "n_tree_model.h"
#include "n_ffd_node_adapter.h"

NIFWIND_NAMESPACE

NMainWindow::NMainWindow()
    : QMainWindow {}
{
    // main menu
    auto miFile = menuBar ()->addMenu ("&File");
        miFile->addAction ("&Open", this, &NMainWindow::HandleFileOpen);
        miFile->addSeparator ();

        auto aQuit = miFile->addAction ("&Quit");
        connect (aQuit, &QAction::triggered, this, &NMainWindow::close);

    auto miHelp = menuBar ()->addMenu ("&Help");
        miHelp->addAction ("&About Qt", qApp, &QApplication::aboutQt);

    //
    auto root = new NFFDNodeAdapter; // can't be shown
    new NFFDNodeAdapter {nullptr, root};
    auto tree = new NTreeModel<NFFDNodeAdapter> (root, this);

    // TODO to a unit test:
    int arr[] = {1,2,3};
    NOptions::PutIntArr ("foo", QVector<int> {arr, arr+3});
    auto arr2 = NOptions::GetIntArr ("foo");
    qDebug () << arr2;

    printf ("arr %d: %d, %d, %d" EOL, arr2.size (), arr2[0], arr2[1], arr2[2]);
    auto tv = new QTreeView {};
    // I have to sub-class QTreeView just to set its initial size as a docked
    // tree?! You have got to be nice mountain view kidding me. TODO read
    // QDockWidget
    tv->setModel (tree);
    auto foo = new QDockWidget {"TreeView"};
    foo->setWidget (tv);
    addDockWidget (Qt::LeftDockWidgetArea, foo);
    // status bar
    statusBar ()->showMessage ("Idle");
}

NMainWindow::~NMainWindow() {}

void NMainWindow::HandleFileOpen()
{
    QFileDialog dlg {this};
    dlg.setFileMode (QFileDialog::ExistingFile);
    dlg.setNameFilter ("Nif files (*.nif *.nifcache *.texcache)");
    dlg.setViewMode (QFileDialog::Detail);
    if (dlg.exec ()) printf ("Open it\n");
    else printf ("Don't open it\n");
}

NAMESPACE_NIFWIND
