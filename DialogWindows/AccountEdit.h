#ifndef ACCOUNTEDIT_H
#define ACCOUNTEDIT_H

#include <QDialog>

namespace Ui {
class AccountEdit;
}

class AccountEdit : public QDialog
{
    Q_OBJECT

public:
    explicit AccountEdit(QWidget *parent = nullptr, const QString& Login = QString(), const QString& InName = QString());
    ~AccountEdit();

signals:
    void UserEdited(const QString& login, const QString& name);

private slots:
    void on_CancelButton_clicked();
    void on_SaveButton_clicked();
    void OnUserUpdateFinished(bool success, const QString& login, const QString& name);

private:
    bool IsPasswordValid(const QString& Password);
    bool IsNameValid(const QString& Name);

    QString UserLogin;
    QString UserOldName;

    Ui::AccountEdit *ui;
};

#endif // ACCOUNTEDIT_H
