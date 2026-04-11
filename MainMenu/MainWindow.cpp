#include "MainWindow.h"
#include "UI/ui_MainWindow.h"

#include "LostConnectionDialog.h"

#include <QDialog>
#include <QLineEdit>

#include "AppConfig.h"
#include "DataBaseManager.h"
#include "LoginLockout.h"
#include "AdminMenu.h"
#include "AnalystMenu.h"
#include "EnergyMenu.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->PasswordLine->setEchoMode(QLineEdit::Password);

    DisableLabels();
    ui->PageWidget->setCurrentIndex(0);

    connect(&DataBaseManager::instance(), &DataBaseManager::LoginVerifyFinished,
            this, &MainWindow::OnLoginVerifyFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::BackToAuthMenu()
{
    const QList<QDialog*> dialogs = findChildren<QDialog*>();
    for (QDialog *dialog : dialogs) {
        if (dialog)
        {
            dialog->close();
        }
    }

    ClearEdits();
    DisableLabels();
    ui->PageWidget->setCurrentIndex(0);
    ResetRoleMenu();

    LostConnectionDialog lostDialog(this);
    lostDialog.exec();
}

void MainWindow::on_LogInButton_clicked()
{

    ui->LogInButton->setEnabled(false);
    
    DisableLabels();

    int remainingSec = 0;
    if (LoginLockout::isLocked(remainingSec)) {
        ui->ErrorLabel->setText(LoginLockout::messageForRemainingSeconds(remainingSec));
        ui->ErrorLabel->setVisible(true);
        ui->LogInButton->setEnabled(true);
        return;
    }

    QString LoginText = ui->LoginLine->text();
    QString PasswordText = ui->PasswordLine->text();

    if (AppConfig::IsSuperAdmin(LoginText))
    {
        if (AppConfig::CheckSAPassword(LoginText, PasswordText))
        {
            LoginLockout::clearOnSuccessfulLogin();

            AdminMenu* Menu = new AdminMenu(this);
            ui->RoleLayout->addWidget(Menu);

            SetUserName(LoginText);

            ui->PageWidget->setCurrentIndex(1);
        }
        else
        {
            ShowCredentialError();
        }
        ui->LogInButton->setEnabled(true);
        return;
    }

    if (LoginText.trimmed().isEmpty() || PasswordText.isEmpty()) {
        EnableWrongPassword();
        ui->LogInButton->setEnabled(true);
        return;
    }

    DataBaseManager::instance().RequestLoginVerify(LoginText.trimmed(), PasswordText);
}

void MainWindow::OnLoginVerifyFinished(bool success, const QString& dbRole, const QString& displayName,
                                       bool invalidCredentials)
{
    ui->LogInButton->setEnabled(true);

    if (!success) {
        if (invalidCredentials) {
            ShowCredentialError();
        }
        return;
    }

    LoginLockout::clearOnSuccessfulLogin();

    const QString userLabel = displayName.trimmed().isEmpty() ? ui->LoginLine->text().trimmed() : displayName;
    SetUserName(userLabel);

    if (dbRole == AppConfig::DatabaseAnalystStr) {
        AnalystMenu* Menu = new AnalystMenu(this);
        ui->RoleLayout->addWidget(Menu);
    } else if (dbRole == AppConfig::DatabaseEnergyStr) {
        EnergyMenu* Menu = new EnergyMenu(this);
        ui->RoleLayout->addWidget(Menu);
    } else {
        ShowCredentialError();
        return;
    }

    ui->PageWidget->setCurrentIndex(1);
}

void MainWindow::on_RecoveryButton_clicked()
{
    DisableLabels();
    ui->RecoveryLabel->setVisible(true);
}

void MainWindow::on_LogOutButton_clicked()
{
    ClearEdits();
    DisableLabels();
    ui->PageWidget->setCurrentIndex(0);
    ResetRoleMenu();
}

void MainWindow::SetUserName(const QString &UName)
{
    ui->UserName->setText(UName);
}

void MainWindow::EnableWrongPassword()
{
    ui->ErrorLabel->setText(QStringLiteral("Неверный логин или пароль"));
    ui->ErrorLabel->setVisible(true);
}

void MainWindow::ShowCredentialError()
{
    LoginLockout::registerInvalidCredentialsAttempt();
    int remainingSec = 0;
    if (LoginLockout::isLocked(remainingSec)) {
        ui->ErrorLabel->setText(LoginLockout::messageForRemainingSeconds(remainingSec));
    } else {
        ui->ErrorLabel->setText(QStringLiteral("Неверный логин или пароль"));
    }
    ui->ErrorLabel->setVisible(true);
}

void MainWindow::ClearEdits()
{
    ui->LoginLine->clear();
    ui->PasswordLine->clear();
}

void MainWindow::DisableLabels()
{
    ui->ErrorLabel->setVisible(false);
    ui->RecoveryLabel->setVisible(false);
}

void MainWindow::ResetRoleMenu()
{
    if (ui->RoleLayout->count() > 0)
    {
        QLayoutItem* Item = ui->RoleLayout->itemAt(0);
        Item->widget()->deleteLater();
    }
}
