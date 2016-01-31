// Copyright (c) 2011-2013 The Darcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRANSACTIONDESCDIALOG_H
#define TRANSACTIONDESCDIALOG_H

#include <QDialog>
#include "msgshowdlg.h"
#include <QTextEdit>

namespace Ui {
    class TransactionDescDialog;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Dialog showing transaction details. */
class TransactionDescDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransactionDescDialog(QWidget *parent = 0);
    ~TransactionDescDialog();
    MsgShowDlg *m_MsgDlg;
    QTextEdit  *m_Edit;
    void SetMode(bool bMode);
private:
    Ui::TransactionDescDialog *ui;
};

#endif // TRANSACTIONDESCDIALOG_H
