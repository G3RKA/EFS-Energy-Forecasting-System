#include "AccountDelete.h"
#include "UI/ui_AccountDelete.h"

#include "AppDataBase.h"

AccountDelete::AccountDelete(QWidget *parent, const QString& InLogin)
    : QDialog(parent)
    , ui(new Ui::AccountDelete)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    UserLogin = InLogin;
    ui->TextLabel->setText(ui->TextLabel->text().arg(InLogin));
}

AccountDelete::~AccountDelete()
{
    delete ui;
}

void AccountDelete::on_ConfirmButton_clicked()
{
    if (!AppDataBase::DeleteUserByLogin(UserLogin)) {
        return;
    }

    emit UserDeleted(UserLogin);
    accept();
}

void AccountDelete::on_CancelButton_clicked()
{
    this->close();
}
