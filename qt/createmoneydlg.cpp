#include "createmoneydlg.h"
#include "ui_createmoneydlg.h"

#include <QMessageBox>

CreateMoneyDlg::CreateMoneyDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateMoneyDlg)
{
    ui->setupUi(this);
}

CreateMoneyDlg::~CreateMoneyDlg()
{
    delete ui;
}

void CreateMoneyDlg::on_pushButton_clicked()
{
    moneyAmount = ui->editMoneyAmount->text().toInt();
    if(moneyAmount<28000000)
    {
       accept();
    }else
    {
        QMessageBox::information(this,tr("提示"),tr("增发货币不能超过28000000"));
    }
}

void CreateMoneyDlg::on_pushButton_2_clicked()
{
    reject();
}
