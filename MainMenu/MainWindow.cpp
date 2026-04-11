#include "MainWindow.h"
#include "UI/ui_MainWindow.h"

#include "LostConnectionDialog.h"

#include <QDialog>
#include <QLineEdit>

#include "AppConfig.h"
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

    QString LoginText = ui->LoginLine->text();
    QString PasswordText = ui->PasswordLine->text();

    if (AppConfig::IsSuperAdmin(LoginText))
    {
        if (AppConfig::CheckSAPassword(LoginText, PasswordText))
        {
            AdminMenu* Menu = new AdminMenu(this);
            ui->RoleLayout->addWidget(Menu);

            SetUserName(LoginText);

            ui->PageWidget->setCurrentIndex(1);
        }
        else
        {
            EnableWrongPassword();
        }
        ui->LogInButton->setEnabled(true);
        return;
    }


    QString Role(ui->LoginLine->text());

    if (Role == "Analyst")
    {
        AnalystMenu* Menu = new AnalystMenu(this);
        ui->RoleLayout->addWidget(Menu);
    }
    else if (Role == "Energy")
    {
        EnergyMenu* Menu = new EnergyMenu(this);
        ui->RoleLayout->addWidget(Menu);
    }

    ui->PageWidget->setCurrentIndex(1);


    ui->LogInButton->setEnabled(true);
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
