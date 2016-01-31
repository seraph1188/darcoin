#ifndef CREATEMONEYDLG_H
#define CREATEMONEYDLG_H

#include <QDialog>

namespace Ui {
class CreateMoneyDlg;
}

class CreateMoneyDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CreateMoneyDlg(QWidget *parent = 0);
    ~CreateMoneyDlg();
    int moneyAmount;
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CreateMoneyDlg *ui;
};

#endif // CREATEMONEYDLG_H
