// Copyright (c) 2011-2013 The Darcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transactiondescdialog.h"
#include "ui_transactiondescdialog.h"

TransactionDescDialog::TransactionDescDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransactionDescDialog)
{
    ui->setupUi(this);
    m_MsgDlg = NULL;
    m_Edit = NULL;
}

TransactionDescDialog::~TransactionDescDialog()
{
    if(m_MsgDlg)
        delete m_MsgDlg;
    if(m_Edit)
        delete m_Edit;
    delete ui;
}

void TransactionDescDialog::SetMode(bool bMode)
{
    if(bMode)
    {
        m_MsgDlg = new MsgShowDlg(this);
        m_MsgDlg->SetMode(true);
        ui->verticalLayout->insertWidget(0,m_MsgDlg);
    }else
    {
        m_Edit = new QTextEdit(this);
        ui->verticalLayout->insertWidget(0,m_Edit);
    }
}
