#ifndef ACCOUNTCREATE_H
#define ACCOUNTCREATE_H

#include <QDialog>

namespace Ui {
class AccountCreate;
}

class AccountCreate : public QDialog
{
    Q_OBJECT

public:
    explicit AccountCreate(QWidget *parent = nullptr);
    ~AccountCreate();

private slots:
    void on_CloseButton_clicked();

private:
    Ui::AccountCreate *ui;
};

#endif // ACCOUNTCREATE_H
