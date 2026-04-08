#ifndef ACCOUNTSETTINGS_H
#define ACCOUNTSETTINGS_H

#include <QDialog>

class QPushButton;
class QString;
class QVariant;

namespace Ui {
class AccountSettings;
}

class AccountSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AccountSettings(QWidget *parent = nullptr);
    ~AccountSettings();

private slots:
    void on_ExitButton_clicked();
    void on_CreateButton_clicked();

private:
    void LoadUsersFromDatabase();
    void EditUserRowByLogin(const QString& login, const QString& name);
    void RemoveUserRowByLogin(const QString& login);
    void SetActionButtonProperties(QPushButton* button, const QString& login);
    void OnEditButtonClicked(QPushButton* button);
    void OnDeleteButtonClicked(QPushButton* button);

    QString FormatLastVisit(const QVariant&);
    QString FormatRole(const QVariant&);

    Ui::AccountSettings *ui;
};

#endif // ACCOUNTSETTINGS_H
