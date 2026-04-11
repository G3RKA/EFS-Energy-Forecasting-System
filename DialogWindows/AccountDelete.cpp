#include "AccountDelete.h"
#include "UI/ui_AccountDelete.h"

#include "DataManager.h"

AccountDelete::AccountDelete(QWidget *Parent, const QString& InLogin)
    : QDialog(Parent)
    , ui(new Ui::AccountDelete)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    UserLogin = InLogin;
    ui->TextLabel->setText(ui->TextLabel->text().arg(InLogin));

    connect(&DataManager::instance(), &DataManager::UserDeleteFinished, this,
            &AccountDelete::OnUserDeleteFinished);
}

AccountDelete::~AccountDelete()
{
    delete ui;
}

void AccountDelete::on_ConfirmButton_clicked()
{
    ui->ConfirmButton->setEnabled(false);
    DataManager::instance().RequestDeleteUser(UserLogin);
}

void AccountDelete::OnUserDeleteFinished(bool Success, const QString& Login)
{
    if (Login != UserLogin) {
        return;
    }
    ui->ConfirmButton->setEnabled(true);
    if (!Success) {
        return;
    }
    emit UserDeleted(UserLogin);
    accept();
}

void AccountDelete::on_CancelButton_clicked()
{
    this->close();
}
