#ifndef ACCOUNTSETTINGS_H
#define ACCOUNTSETTINGS_H

#include <QDialog>
#include <QVector>

#include "DataTypes/AccountUserRow.h"

class QPushButton;

namespace Ui {
class AccountSettings;
}

class AccountSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AccountSettings(QWidget *Parent = nullptr);
    ~AccountSettings();

private slots:
    void on_ExitButton_clicked();
    void on_CreateButton_clicked();

private:
    void LoadUsersFromDatabase();
    void LoadUsers(const QVector<AccountUserRow>& Users);
    void EditUserRowByLogin(const QString& Login, const QString& Name);
    void RemoveUserRowByLogin(const QString& Login);
    void SetActionButtonProperties(QPushButton* Button, const QString& Login);
    void OnEditButtonClicked(QPushButton* Button);
    void OnDeleteButtonClicked(QPushButton* Button);

    QString FormatLastVisit(const QVariant&);
    QString FormatRole(const QVariant&);

    Ui::AccountSettings *ui;
};

#endif // ACCOUNTSETTINGS_H
