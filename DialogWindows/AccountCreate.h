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

signals:
    void tableUpdateRequested();

private slots:
    void on_CloseButton_clicked();
    void on_ConnectButton_clicked();

private:
    bool IsLoginValid(const QString& Login);
    bool IsPasswordValid(const QString& Password);
    bool IsNameValid(const QString& Name);

    Ui::AccountCreate *ui;
};

#endif // ACCOUNTCREATE_H
