#include "AccountCreate.h"
#include "UI/ui_AccountCreate.h"

#include "AppDataBase.h"
#include "bcrypt.hpp"

#include <QRegularExpression>

AccountCreate::AccountCreate(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccountCreate)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->ErrorLabel->setVisible(false);

    ui->NameEdit->setMaxLength(45);
    ui->LoginEdit->setMaxLength(30);
}

AccountCreate::~AccountCreate()
{
    delete ui;
}

void AccountCreate::on_ConnectButton_clicked()
{
    const QString login = ui->LoginEdit->text().trimmed();
    const QString name = ui->NameEdit->text().trimmed();
    const QString pass = ui->PassEdit->text().trimmed();

    QStringList failed;
    if (AppDataBase::IsLoginTaken(login)) {
        failed << "Логин должен быть уникальным";
    }
    if (!IsLoginValid(login)) {
        failed << "Логин должен состоять только из букв латинского алфавита и цифр";
    } 
    if (!IsNameValid(name)) 
    {
        failed << "Имя должно состоять только из букв русского или латинского алфавита";
    }
    if (!IsPasswordValid(pass)) 
    {
        failed << "Пароль должен быть не менее 8 символов и содержать буквы латинского алфавита в верхнем, нижнем регистрах и цифры";
    }

    if (!failed.isEmpty()) {
        QString message = failed.join(".\n");
        message += '.';
        ui->ErrorLabel->setText(message);
        ui->ErrorLabel->setVisible(true);
        return;
    }

    const QString passhash = QString::fromStdString(BCrypt::generateHash(pass.toStdString()));

    ui->ErrorLabel->setVisible(false);
    if (!AppDataBase::CreateUser(login, name, ui->RoleBox->currentText(), passhash)) {
        return;
    }

    emit tableUpdateRequested();
    accept();
}

void AccountCreate::on_CloseButton_clicked()
{
    this->close();
}

bool AccountCreate::IsLoginValid(const QString& Login)
{
    const QRegularExpression pattern(R"(^[A-Za-z0-9]+$)");
    return pattern.match(Login).hasMatch();
}

bool AccountCreate::IsPasswordValid(const QString& Password)
{
    const QRegularExpression pattern(R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,}$)");
    return pattern.match(Password).hasMatch();
}

bool AccountCreate::IsNameValid(const QString& Name)
{
    static const QRegularExpression pattern(R"(^[\p{Latin}\p{Cyrillic}]{1,45}$)",
        QRegularExpression::UseUnicodePropertiesOption);
    return pattern.match(Name).hasMatch();
}
