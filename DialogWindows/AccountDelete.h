#ifndef ACCOUNTDELETE_H
#define ACCOUNTDELETE_H

#include <QDialog>

namespace Ui {
class AccountDelete;
}

class AccountDelete : public QDialog
{
    Q_OBJECT

public:
    explicit AccountDelete(QWidget *Parent = nullptr, const QString& InLogin = QString());
    ~AccountDelete();

signals:
    void UserDeleted(const QString& Login);

private slots:
    void on_ConfirmButton_clicked();
    void on_CancelButton_clicked();
    void OnUserDeleteFinished(bool Success, const QString& Login);

private:
    QString UserLogin;

    Ui::AccountDelete *ui;
};

#endif // ACCOUNTDELETE_H
