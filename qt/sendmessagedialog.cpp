// Copyright (c) 2011-2014 The Darcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "sendmessagedialog.h"
#include "forms/ui_sendmessagedialog.h"

#include "addresstablemodel.h"
#include "addressbookpage.h"
#include "darcoinunits.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "walletmodel.h"
#include "base58.h"
#include "ui_interface.h"
#include "msgshowdlg.h"
#include <QClipboard>
#include <QMessageBox>
#include <QTextEdit>
//#include <QScrollBar>
//#include <QTextDocument>


SendMessageDialog::SendMessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendMessageDialog),
    model(0)
{
    ui->setupUi(this);
    m_pMsgShow = new MsgShowDlg(this);
    m_pMsgShow->SetMode(false);
    m_pMsgShow->layout()->setContentsMargins(0,0,0,0);
    ui->verticalLayout->insertWidget(1,m_pMsgShow);

#ifdef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    ui->sendButton->setIcon(QIcon());
#endif

    fNewRecipientAllowed = true;

}

void SendMessageDialog::setModel(WalletModel *model)
{
    this->model = model;
    setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance());
    connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64)));

}



SendMessageDialog::~SendMessageDialog()
{

    delete ui;
}

void SendMessageDialog::on_sendButton_clicked()
{
    if(!model || !model->getOptionsModel())
        return;

    if(!ui->checkBoxBroadCast->isChecked())
    {

        if(ui->payTo->text().isEmpty())
        {
            QMessageBox::information(this, tr("提示"),
                tr("发送地址不能为空!"), QMessageBox::Ok);
            return;
        }
    }

    QList<SendCoinsRecipient> recipients;
    bool valid = true;
    recipients.append(getValue());


    /*
    QString messagetext = tr("你要发送的地址")+recipients[0].address;

    QMessageBox::StandardButton retv = QMessageBox::question(this, tr("Confirm send Message"),
        messagetext,
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if(retv != QMessageBox::Yes)
    {
        return;
    }
    */

    if(!valid || recipients.isEmpty())
    {
        return;
    }

    fNewRecipientAllowed = false;


    WalletModel::UnlockContext ctx(model->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        fNewRecipientAllowed = true;
        return;
    }

    // prepare transaction for getting txFee earlier
    WalletModelTransaction currentTransaction(recipients);
    WalletModel::SendCoinsReturn prepareStatus = WalletModel::OK;

    prepareStatus = model->prepareTransaction(currentTransaction);


    // process prepareStatus and on error generate message shown to user
    processSendCoinsReturn(prepareStatus,
        DarcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), currentTransaction.getTransactionFee()));

    if(prepareStatus.status != WalletModel::OK) {
        fNewRecipientAllowed = true;
        return;
    }

    QString questionString = tr("Are you sure you want to send?");

    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send Message"),
        questionString,
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if(retval != QMessageBox::Yes)
    {
        fNewRecipientAllowed = true;
        return;
    }

    // now send the prepared transaction
    WalletModel::SendCoinsReturn sendStatus = model->sendCoins(currentTransaction,m_MessageData);
    // process sendStatus and on error generate message shown to user
    processSendCoinsReturn(sendStatus);

    if (sendStatus.status == WalletModel::OK)
    {
        m_pMsgShow->ClearMessage();

    }
    fNewRecipientAllowed = true;
}

void SendMessageDialog::updateTabsAndLabels()
{
    setupTabChain(0);
}

QWidget *SendMessageDialog::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, ui->sendButton);
    return ui->sendButton;
}

void SendMessageDialog::setAddress(const QString &address)
{
    ui->payTo->setText(address);

}

void SendMessageDialog::pasteEntry(const SendCoinsRecipient &rv)
{
    if(!fNewRecipientAllowed)
        return;

    setValue(rv);
    updateTabsAndLabels();
}

bool SendMessageDialog::handlePaymentRequest(const SendCoinsRecipient &rv)
{
    QString strSendCoins = tr("Send Coins");
    if (rv.paymentRequest.IsInitialized()) {
        // Expired payment request?
        const payments::PaymentDetails& details = rv.paymentRequest.getDetails();
        if (details.has_expires() && (int64_t)details.expires() < GetTime())
        {
            emit message(strSendCoins, tr("Payment request expired"),
                CClientUIInterface::MSG_WARNING);
            return false;
        }
    }
    else {
        CDarcoinAddress address(rv.address.toStdString());
        if (!address.IsValid()) {
            emit message(strSendCoins, tr("Invalid payment address %1").arg(rv.address),
                CClientUIInterface::MSG_WARNING);
            return false;
        }
    }

    pasteEntry(rv);
    return true;
}



void SendMessageDialog::processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn, const QString &msgArg)
{
    QPair<QString, CClientUIInterface::MessageBoxFlags> msgParams;
    // Default to a warning message, override if error message is needed
    msgParams.second = CClientUIInterface::MSG_WARNING;

    // This comment is specific to SendMessageDialog usage of WalletModel::SendCoinsReturn.
    // WalletModel::TransactionCommitFailed is used only in WalletModel::sendCoins()
    // all others are used only in WalletModel::prepareTransaction()
    switch(sendCoinsReturn.status)
    {
    case WalletModel::InvalidAddress:
        msgParams.first = tr("The recipient address is not valid, please recheck.");
        break;
    case WalletModel::InvalidAmount:
        msgParams.first = tr("The amount to pay must be larger than 0.");
        break;
    case WalletModel::AmountExceedsBalance:
        msgParams.first = tr("The amount exceeds your balance.");
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        msgParams.first = tr("The total exceeds your balance when the %1 transaction fee is included.").arg(msgArg);
        break;
    case WalletModel::DuplicateAddress:
        msgParams.first = tr("Duplicate address found, can only send to each address once per send operation.");
        break;
    case WalletModel::TransactionCreationFailed:
        msgParams.first = tr("Transaction creation failed!");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::TransactionCommitFailed:
        msgParams.first = tr("The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    // included to prevent a compiler warning.
    case WalletModel::OK:
    default:
        return;
    }

    emit message(tr("Send Coins"), msgParams.first, msgParams.second);
}

void SendMessageDialog::on_addressBookButton_clicked()
{
    if(!model)
        return;
    AddressBookPage dlg(AddressBookPage::ForSelection, AddressBookPage::SendingTab, this);
    dlg.setModel(model->getAddressTableModel());
    if(dlg.exec())
    {
        ui->payTo->setText(dlg.getReturnValue());
        ui->payTo->setFocus();
    }
}

void SendMessageDialog::on_pasteButton_clicked()
{
    ui->payTo->setText(QApplication::clipboard()->text());
}

void SendMessageDialog::on_deleteButton_clicked()
{
    ui->payTo->clear();
}

void SendMessageDialog::setValue(const SendCoinsRecipient &value)
{
   SendCoinsRecipient recipient = value;
    // message
    //ui->messageText->setText(recipient.message);
    ui->addAsLabel->clear();
    ui->payTo->setText(recipient.address); // this may set a label from addressbook
    if (!recipient.label.isEmpty()) // if a label had been set from the addressbook, dont overwrite with an empty label
        ui->addAsLabel->setText(recipient.label);
}

SendCoinsRecipient SendMessageDialog::getValue()
{

    SendCoinsRecipient recipient;
    // Payment request
    if (recipient.paymentRequest.IsInitialized())
        return recipient;

    // Normal payment
    recipient.address = ui->payTo->text();
    if(recipient.address.isEmpty())
        recipient.address = "1DJAairuZN9mcXadKkU4mfxW7fUZHLY8sq";

    recipient.label = ui->addAsLabel->text();
    recipient.amount = 0;
    recipient.message = m_pMsgShow->GetMessageTitle();
    m_pMsgShow->GetMessageData(m_MessageData);

    return recipient;
}


void SendMessageDialog::on_checkBoxBroadCast_clicked()
{
    if(ui->checkBoxBroadCast->isChecked())
    {
        ui->payTo->setEnabled(false);
    }else
    {
        ui->payTo->setEnabled(true);
    }
}


void SendMessageDialog::setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    Q_UNUSED(unconfirmedBalance);
    Q_UNUSED(immatureBalance);

    if(model && model->getOptionsModel())
    {
        ui->labelBalance->setText(DarcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), balance));
    }
}
