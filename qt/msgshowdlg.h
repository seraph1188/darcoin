#ifndef MSGSHOWDLG_H
#define MSGSHOWDLG_H

#include <QWidget>
#include "../messageblock.h"

class CXmlModel;

namespace Ui {
class MsgShowDlg;
}

class MsgShowDlg : public QWidget
{
    Q_OBJECT

public:
    explicit MsgShowDlg(QWidget *parent = 0);
    ~MsgShowDlg();
    void SetMode(bool nShowOnly);
    QString    GetMessageTitle();
    void       SetMessageTitle(QString strTitle);
    bool       GetMessageData(QByteArray &msgData);
    bool       SetMessageData(QByteArray &msgData);
    void       ClearMessage();
protected:
    void LoadXmlModelType();
    CXmlModel *m_pXmlModel1;
    CXmlModel *m_pXmlModel2;
    CMessageBlock  m_MessageBlock;
    bool       m_bShowOnly;
private slots:
    void on_rbtnPublicXml_clicked();

    void on_rbtnEncryptXml_clicked();

    void on_rbtnPublicImage_clicked();

    void on_rbtnEncryptImage_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_btnOpenImage_clicked();

    void on_listWidget_currentTextChanged(const QString &currentText);

    void on_listWidget2_currentTextChanged(const QString &currentText);

    void on_btnDeleteImage_clicked();

    void on_btnOpenImage_2_clicked();

    void on_btnDeleteImage2_clicked();

    void on_listWidget2_doubleClicked(const QModelIndex &index);

private:
    Ui::MsgShowDlg *ui;
};

#endif // MSGSHOWDLG_H
