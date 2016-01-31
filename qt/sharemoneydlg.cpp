#include "sharemoneydlg.h"
#include "ui_sharemoneydlg.h"

#include <QMessageBox>

ShareMoneyDlg::ShareMoneyDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareMoneyDlg)
{
    ui->setupUi(this);
    sharepercent = 0;
    freemoney = 0;
    punishpercent = 0;
}

ShareMoneyDlg::~ShareMoneyDlg()
{
    delete ui;
}

void ShareMoneyDlg::on_pushButton_clicked()
{
    sharepercent = ui->editSharePercent->text().toFloat();
    if(sharepercent>0 && sharepercent<10000)
    {
        accept();
    }else
    {
        QMessageBox::information(this,tr("提示"),tr("输入的数值不正确"));
    }
}

void ShareMoneyDlg::on_pushButton_2_clicked()
{
    reject();
}

void ShareMoneyDlg::on_edtFreeMoney_textChanged(const QString &arg1)
{
    freemoney = arg1.toDouble();
    sharepercent = freemoney*punishpercent*100*0.7/26175871;
    ui->editSharePercent->setText(QString::number(sharepercent,'g',4));
}

void ShareMoneyDlg::on_edtPunishPercent_textChanged(const QString &arg1)
{
    punishpercent = arg1.toDouble();
    sharepercent = freemoney*punishpercent*100*0.7/26175871;
    ui->editSharePercent->setText(QString::number(sharepercent,'g',4));
}
