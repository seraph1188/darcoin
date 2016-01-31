// Copyright (c) 2011-2013 The Darcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "walletview.h"

#include "addressbookpage.h"
#include "askpassphrasedialog.h"
#include "darcoingui.h"
#include "clientmodel.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "sendcoinsdialog.h"
#include "sendmessagedialog.h"
#include "signverifymessagedialog.h"
#include "transactiontablemodel.h"
#include "transactionview.h"
#include "walletmodel.h"
#include "sharemoneydlg.h"
#include "createmoneydlg.h"
#include "../messageblock.h"
#include "darcoinunits.h"
#include "ui_interface.h"

#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QPushButton>
#include <QVBoxLayout>

WalletView::WalletView(QWidget *parent):
    QStackedWidget(parent),
    clientModel(0),
    walletModel(0)
{
    // Create tabs
    transactionsPage = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout();
    QHBoxLayout *hbox_buttons = new QHBoxLayout();
    transactionView = new TransactionView(this);
    vbox->addWidget(transactionView);
    QPushButton *exportButton = new QPushButton(tr("&Export"), this);
    exportButton->setToolTip(tr("Export the data in the current tab to a file"));
#ifndef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    exportButton->setIcon(QIcon(":/icons/export"));
#endif
    hbox_buttons->addStretch();
    hbox_buttons->addWidget(exportButton);
    vbox->addLayout(hbox_buttons);
    transactionsPage->setLayout(vbox);

    sendCoinsPage = new SendCoinsDialog();


    addWidget(transactionsPage);
    addWidget(sendCoinsPage);
    // Double-clicking on a transaction on the transaction history page shows details
    connect(transactionView, SIGNAL(doubleClicked(QModelIndex)), transactionView, SLOT(showDetails()));

    // Clicking on "Export" allows to export the transaction list
    connect(exportButton, SIGNAL(clicked()), transactionView, SLOT(exportClicked()));

    // Pass through messages from sendCoinsPage
    connect(sendCoinsPage, SIGNAL(message(QString,QString,unsigned int)), this, SIGNAL(message(QString,QString,unsigned int)));

    // Pass through messages from transactionView
    connect(transactionView, SIGNAL(message(QString,QString,unsigned int)), this, SIGNAL(message(QString,QString,unsigned int)));
}

WalletView::~WalletView()
{
}

void WalletView::setDarcoinGUI(DarcoinGUI *gui)
{
    if (gui)
    { 
        // Receive and report messages
        connect(this, SIGNAL(message(QString,QString,unsigned int)), gui, SLOT(message(QString,QString,unsigned int)));

        // Pass through encryption status changed signals
        connect(this, SIGNAL(encryptionStatusChanged(int)), gui, SLOT(setEncryptionStatus(int)));

        // Pass through transaction notifications
        connect(this, SIGNAL(incomingTransaction(QString,int,qint64,QString,QString)), gui, SLOT(incomingTransaction(QString,int,qint64,QString,QString)));
    }
}

void WalletView::setClientModel(ClientModel *clientModel)
{
    this->clientModel = clientModel;

}

void WalletView::setWalletModel(WalletModel *walletModel)
{
    this->walletModel = walletModel;

    // Put transaction list in tabs
    transactionView->setModel(walletModel);
    sendCoinsPage->setModel(walletModel);

    if (walletModel)
    {
        // Receive and pass through messages from wallet model
        connect(walletModel, SIGNAL(message(QString,QString,unsigned int)), this, SIGNAL(message(QString,QString,unsigned int)));

        // Handle changes in encryption status
        connect(walletModel, SIGNAL(encryptionStatusChanged(int)), this, SIGNAL(encryptionStatusChanged(int)));
        updateEncryptionStatus();

        // Balloon pop-up for new transaction
        connect(walletModel->getTransactionTableModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(processNewTransaction(QModelIndex,int,int)));

        // Ask for passphrase if needed
        connect(walletModel, SIGNAL(requireUnlock()), this, SLOT(unlockWallet()));

        // Show progress dialog
        connect(walletModel, SIGNAL(showProgress(QString,int)), this, SLOT(showProgress(QString,int)));
    }
}

void WalletView::processNewTransaction(const QModelIndex& parent, int start, int /*end*/)
{
    // Prevent balloon-spam when initial block download is in progress
    if (!walletModel || !clientModel || clientModel->inInitialBlockDownload())
        return;

    TransactionTableModel *ttm = walletModel->getTransactionTableModel();

    QString date = ttm->index(start, TransactionTableModel::Date, parent).data().toString();
    qint64 amount = ttm->index(start, TransactionTableModel::Amount, parent).data(Qt::EditRole).toULongLong();
    QString type = ttm->index(start, TransactionTableModel::Type, parent).data().toString();
    QString address = ttm->index(start, TransactionTableModel::ToAddress, parent).data().toString();

    emit incomingTransaction(date, walletModel->getOptionsModel()->getDisplayUnit(), amount, type, address);
}


void WalletView::gotoHistoryPage()
{
    setCurrentWidget(transactionsPage);
}


void WalletView::gotoSendCoinsPage(QString addr)
{
    setCurrentWidget(sendCoinsPage);

    if (!addr.isEmpty())
        sendCoinsPage->setAddress(addr);
}

void WalletView::gotoSendMessagePage(QString addr)
{

}

void WalletView::gotoSignMessageTab(QString addr)
{
    // calls show() in showTab_SM()
    SignVerifyMessageDialog *signVerifyMessageDialog = new SignVerifyMessageDialog(this);
    signVerifyMessageDialog->setAttribute(Qt::WA_DeleteOnClose);
    signVerifyMessageDialog->setModel(walletModel);
    signVerifyMessageDialog->showTab_SM(true);

    if (!addr.isEmpty())
        signVerifyMessageDialog->setAddress_SM(addr);
}

void WalletView::gotoVerifyMessageTab(QString addr)
{
    // calls show() in showTab_VM()
    SignVerifyMessageDialog *signVerifyMessageDialog = new SignVerifyMessageDialog(this);
    signVerifyMessageDialog->setAttribute(Qt::WA_DeleteOnClose);
    signVerifyMessageDialog->setModel(walletModel);
    signVerifyMessageDialog->showTab_VM(true);

    if (!addr.isEmpty())
        signVerifyMessageDialog->setAddress_VM(addr);
}

bool WalletView::handlePaymentRequest(const SendCoinsRecipient& recipient)
{
    return sendCoinsPage->handlePaymentRequest(recipient);
}

void WalletView::showOutOfSyncWarning(bool fShow)
{

}

void WalletView::updateEncryptionStatus()
{
    emit encryptionStatusChanged(walletModel->getEncryptionStatus());
}

void WalletView::encryptWallet(bool status)
{
    if(!walletModel)
        return;
    AskPassphraseDialog dlg(status ? AskPassphraseDialog::Encrypt : AskPassphraseDialog::Decrypt, this);
    dlg.setModel(walletModel);
    dlg.exec();

    updateEncryptionStatus();
}

void WalletView::backupWallet()
{
    QString filename = GUIUtil::getSaveFileName(this,
        tr("Backup Wallet"), QString(),
        tr("Wallet Data (*.dat)"), NULL);

    if (filename.isEmpty())
        return;

    if (!walletModel->backupWallet(filename)) {
        emit message(tr("Backup Failed"), tr("There was an error trying to save the wallet data to %1.").arg(filename),
            CClientUIInterface::MSG_ERROR);
        }
    else {
        emit message(tr("Backup Successful"), tr("The wallet data was successfully saved to %1.").arg(filename),
            CClientUIInterface::MSG_INFORMATION);
    }
}

void WalletView::changePassphrase()
{
    AskPassphraseDialog dlg(AskPassphraseDialog::ChangePass, this);
    dlg.setModel(walletModel);
    dlg.exec();
}

void WalletView::unlockWallet()
{
    if(!walletModel)
        return;
    // Unlock wallet when requested by wallet model
    if (walletModel->getEncryptionStatus() == WalletModel::Locked)
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::Unlock, this);
        dlg.setModel(walletModel);
        dlg.exec();
    }
}

void WalletView::usedSendingAddresses()
{
    if(!walletModel)
        return;
    AddressBookPage *dlg = new AddressBookPage(AddressBookPage::ForEditing, AddressBookPage::SendingTab, this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModel(walletModel->getAddressTableModel());
    dlg->show();
}

void WalletView::usedReceivingAddresses()
{
    if(!walletModel)
        return;
    AddressBookPage *dlg = new AddressBookPage(AddressBookPage::ForEditing, AddressBookPage::ReceivingTab, this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModel(walletModel->getAddressTableModel());
    dlg->show();
}

void WalletView::ShowCreateMoneyForm()
{
    if(!walletModel)
        return;

    CreateMoneyDlg dlg;

    dlg.setModal(true);
    dlg.exec();
    if(dlg.result()==QDialog::Accepted)
    {
        if(walletModel->CreateMoneyTrancaction("1NpdgRALUhJ8112KhxrHKt7E39dFwM3D4e",dlg.moneyAmount*COIN))
            QMessageBox::information(this,"增发","增发货币成功");
        else
            QMessageBox::information(this,"增发","增发货币失败");
    }
}

void WalletView::ShowShareMoneyForm()
{
    time_t timer = time(NULL);
    struct tm tmtx;
    localtime_r(&timer,&tmtx);
    bool bShareMoney = false;
    if(tmtx.tm_mon+1==6 || tmtx.tm_mon+1==1)
    {
        if(tmtx.tm_mday<4)
        {
            if(tmtx.tm_mday==1)
            {
                if(tmtx.tm_hour>=2)
                      bShareMoney = true;
            }else
                bShareMoney= true;
        }
    }

    if(!bShareMoney)
    {
        QMessageBox::information(this,tr("分红"),tr("分红时间必须为1月1日至1月3日或6月1日至6月3日期间\n，如果是1日，则必须为凌晨2点以后"));
        return;
    }


    ShareMoneyDlg dlg;
    dlg.setModal(true);
    dlg.exec();
    if(dlg.result()==QDialog::Accepted)
    {
        SendCoinsRecipient recipient;
        QList<SendCoinsRecipient> recipients;
        QByteArray MessageData;
        CMessageBlock msgblock;
        msgblock.nType = 0;
        msgblock.strXmlPublic = "利润总额:"+QString::number(dlg.freemoney,'g',6).toStdString()+"\n"
                +"发行比（百分之）:"+QString::number(dlg.punishpercent,'g',6).toStdString()+"\n"
                +"分红比(万分之):"+QString::number(dlg.sharepercent,'g',6).toStdString();

        CDataStream ss(SER_NETWORK,PROTOCOL_VERSION);
        ss << msgblock;
        MessageData.resize(ss.size());
        memcpy(MessageData.data(),&(*ss.begin()),ss.size());

        // Normal payment
        recipient.address = "1DJAairuZN9mcXadKkU4mfxW7fUZHLY8sq";
        recipient.amount = 0;
        recipient.message = tr("分红通知");
        recipients.append(recipient);

        WalletModel::UnlockContext ctx(walletModel->requestUnlock());
        if(!ctx.isValid())
        {
            // Unlock wallet was cancelled
            return;
        }

        // prepare transaction for getting txFee earlier
        WalletModelTransaction currentTransaction(recipients);
        WalletModel::SendCoinsReturn prepareStatus = WalletModel::OK;

        prepareStatus = walletModel->prepareTransaction(currentTransaction);

        // process prepareStatus and on error generate message shown to user
        //processSendCoinsReturn(prepareStatus,
        //    DarcoinUnits::formatWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), currentTransaction.getTransactionFee()));

        if(prepareStatus.status != WalletModel::OK) {
            return;
        }

        // now send the prepared transaction
        WalletModel::SendCoinsReturn sendStatus = walletModel->sendCoins(currentTransaction,MessageData);
        if(sendStatus.status == WalletModel::OK)
            QMessageBox::information(this,tr("分红"),tr("发送分红信息成功"));
        else
            QMessageBox::information(this,tr("分红"),tr("发送分红信息失败"));
    }

}

void WalletView::showProgress(const QString &title, int nProgress)
{
    if (nProgress == 0)
    {
        progressDialog = new QProgressDialog(title, "", 0, 100);
        progressDialog->setWindowModality(Qt::ApplicationModal);
        progressDialog->setMinimumDuration(0);
        progressDialog->setCancelButton(0);
        progressDialog->setAutoClose(false);
        progressDialog->setValue(0);
    }
    else if (nProgress == 100)
    {
        if (progressDialog)
        {
            progressDialog->close();
            progressDialog->deleteLater();
        }
    }
    else if (progressDialog)
        progressDialog->setValue(nProgress);
}
