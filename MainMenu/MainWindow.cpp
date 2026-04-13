#include "MainWindow.h"
#include "UI/ui_MainWindow.h"

#include "LostConnectionDialog.h"

#include <QDialog>
#include <QLineEdit>

#include "AppConfig.h"
#include "DataManager.h"
#include "LoginLockout.h"
#include "AdminMenu.h"
#include "AnalystMenu.h"
#include "EnergyMenu.h"

MainWindow::MainWindow(QWidget *Parent)
    : QMainWindow(Parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->PasswordLine->setEchoMode(QLineEdit::Password);

    DisableLabels();
    ui->PageWidget->setCurrentIndex(0);

    connect(&DataManager::instance(), &DataManager::LoginVerifyFinished,
            this, &MainWindow::OnLoginVerifyFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::BackToAuthMenu()
{
    const QList<QDialog*> Dialogs = findChildren<QDialog*>();
    for (QDialog *Dialog : Dialogs) {
        if (Dialog)
        {
            Dialog->close();
        }
    }

    ClearEdits();
    DisableLabels();
    ui->PageWidget->setCurrentIndex(0);
    ResetRoleMenu();
    ClearRoleSession();

    LostConnectionDialog LostDialog(this);
    LostDialog.exec();
}

void MainWindow::on_LogInButton_clicked()
{

    ui->LogInButton->setEnabled(false);
    
    DisableLabels();

    int RemainingSec = 0;
    if (LoginLockout::IsLocked(RemainingSec)) {
        ui->ErrorLabel->setText(LoginLockout::MessageForRemainingSeconds(RemainingSec));
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
            LoginLockout::ClearOnSuccessfulLogin();
            ClearRoleSession();

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

    DataManager::instance().RequestLoginVerify(LoginText.trimmed(), PasswordText);
}

void MainWindow::OnLoginVerifyFinished(bool Success, const QString& DbRole, const QString& DisplayName,
                                       bool InvalidCredentials)
{
    ui->LogInButton->setEnabled(true);

    if (!Success) {
        if (InvalidCredentials) {
            ShowCredentialError();
        }
        return;
    }

    LoginLockout::ClearOnSuccessfulLogin();

    const QString UserLabel = DisplayName.trimmed().isEmpty() ? ui->LoginLine->text().trimmed() : DisplayName;
    SetUserName(UserLabel);

    const QString LoginTrimmed = ui->LoginLine->text().trimmed();
    m_SessionLogin = LoginTrimmed;
    m_SessionRoleDb = DbRole;

    if (DbRole == AppConfig::DatabaseAnalystStr) {
        if (AppConfig::IsAuditAnalystLoginLogoutEnabled()) {
            DataManager::instance().RequestAppendAuditLog(LoginTrimmed, "Вход в систему");
        }
        AnalystMenu* Menu = new AnalystMenu(LoginTrimmed, this);
        ui->RoleLayout->addWidget(Menu);
    } else if (DbRole == AppConfig::DatabaseEnergyStr) {
        if (AppConfig::IsAuditEnergyLoginLogoutEnabled()) {
            DataManager::instance().RequestAppendAuditLog(LoginTrimmed, "Вход в систему");
        }
        EnergyMenu* Menu = new EnergyMenu(this);
        ui->RoleLayout->addWidget(Menu);
    } else {
        ShowCredentialError();
        ClearRoleSession();
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
    TryAppendLogoutAudit();

    ClearEdits();
    DisableLabels();
    ui->PageWidget->setCurrentIndex(0);
    ResetRoleMenu();
    ClearRoleSession();
}

void MainWindow::TryAppendLogoutAudit()
{
    if (m_SessionLogin.isEmpty()) {
        return;
    }
    if (m_SessionRoleDb == AppConfig::DatabaseAnalystStr && AppConfig::IsAuditAnalystLoginLogoutEnabled()) {
        DataManager::instance().RequestAppendAuditLog(m_SessionLogin, QStringLiteral("Выход из системы"));
    } else if (m_SessionRoleDb == AppConfig::DatabaseEnergyStr && AppConfig::IsAuditEnergyLoginLogoutEnabled()) {
        DataManager::instance().RequestAppendAuditLog(m_SessionLogin, QStringLiteral("Выход из системы"));
    }
}

void MainWindow::ClearRoleSession()
{
    m_SessionLogin.clear();
    m_SessionRoleDb.clear();
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
    LoginLockout::RegisterInvalidCredentialsAttempt();
    int RemainingSec = 0;
    if (LoginLockout::IsLocked(RemainingSec)) {
        ui->ErrorLabel->setText(LoginLockout::MessageForRemainingSeconds(RemainingSec));
    } else {
        ui->ErrorLabel->setText(QString("Неверный логин или пароль"));
    }
    ui->ErrorLabel->setVisible(true);
}

void MainWindow::ClearEdits()
{
    ui->LoginLine->clear();
    ui->PasswordLine->clear();
    ui->LogInButton->setEnabled(true);
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
