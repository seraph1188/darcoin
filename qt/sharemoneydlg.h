#ifndef SHAREMONEYDLG_H
#define SHAREMONEYDLG_H

#include <QDialog>

namespace Ui {
class ShareMoneyDlg;
}

class ShareMoneyDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ShareMoneyDlg(QWidget *parent = 0);
    ~ShareMoneyDlg();
    double sharepercent;
    double freemoney;
    double punishpercent;
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_edtFreeMoney_textChanged(const QString &arg1);

    void on_edtPunishPercent_textChanged(const QString &arg1);

private:
    Ui::ShareMoneyDlg *ui;
};

#endif // SHAREMONEYDLG_H
