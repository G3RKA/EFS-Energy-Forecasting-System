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
    explicit AccountDelete(QWidget *parent = nullptr, const QString& login = QString());
    ~AccountDelete();

signals:
    void UserDeleted(const QString& login);

private slots:
    void on_ConfirmButton_clicked();
    void on_CancelButton_clicked();
    void OnUserDeleteFinished(bool success, const QString& login);

private:
    QString UserLogin;

    Ui::AccountDelete *ui;
};

#endif // ACCOUNTDELETE_H
