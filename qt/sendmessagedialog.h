// Copyright (c) 2011-2013 The Darcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SENDMESSAGEDIALOG_H
#define SENDMESSAGEDIALOG_H

#include "walletmodel.h"

#include <QDialog>
#include <QString>


class OptionsModel;
class SendCoinsRecipient;
class MsgShowDlg;

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

namespace Ui {
    class SendMessageDialog;
}

/** Dialog for sending darcoins */
class SendMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendMessageDialog(QWidget *parent = 0);
    ~SendMessageDialog();

    void setModel(WalletModel *model);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907).
     */
    QWidget *setupTabChain(QWidget *prev);

    void setAddress(const QString &address);
    void pasteEntry(const SendCoinsRecipient &rv);
    bool handlePaymentRequest(const SendCoinsRecipient &recipient);
    SendCoinsRecipient getValue();
    void setValue(const SendCoinsRecipient &value);
private:
    Ui::SendMessageDialog *ui;
    WalletModel *model;
    MsgShowDlg  *m_pMsgShow;
    QByteArray  m_MessageData;
    bool fNewRecipientAllowed;

    // Process WalletModel::SendCoinsReturn and generate a pair consisting
    // of a message and message flags for use in emit message().
    // Additional parameter msgArg can be used via .arg(msgArg).
    void processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn, const QString &msgArg = QString());
    void updateTabsAndLabels();
private slots:
    void on_sendButton_clicked();
    void on_addressBookButton_clicked();
    void on_pasteButton_clicked();
    void on_deleteButton_clicked();
    void setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance);
    void on_checkBoxBroadCast_clicked();

signals:
    // Fired when a message should be reported to the user
    void message(const QString &title, const QString &message, unsigned int style);
};

#endif // SENDMESSAGEDIALOG_H
