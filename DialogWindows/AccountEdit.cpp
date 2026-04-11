#include "AccountEdit.h"
#include "UI/ui_AccountEdit.h"

#include "DataManager.h"

#include <QLineEdit>
#include <QRegularExpression>

#include "bcrypt.hpp"

AccountEdit::AccountEdit(QWidget *parent, const QString& InLogin, const QString& InName)
    : QDialog(parent)
    , ui(new Ui::AccountEdit)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    UserLogin = InLogin;
    UserOldName = InName;

    ui->NameEdit->setMaxLength(45);
    ui->NameEdit->setText(UserOldName);

    ui->ErrorLabel->setVisible(false);

    connect(&DataManager::instance(), &DataManager::UserUpdateFinished, this,
            &AccountEdit::OnUserUpdateFinished);
}

AccountEdit::~AccountEdit()
{
    delete ui;
}

void AccountEdit::on_SaveButton_clicked()
{
    ui->ErrorLabel->setVisible(false);

    const QString name = ui->NameEdit->text().trimmed();
    const QString pass = ui->PassEdit->text().trimmed();
    const bool nameChanged = (name != UserOldName);

    if (nameChanged and !IsNameValid(name)) {
        ui->ErrorLabel->setText("Имя должно состоять только из букв русского или латинского алфавита.");
        ui->ErrorLabel->setVisible(true);
        return;
    }

    if (!pass.isEmpty() and !IsPasswordValid(pass)) {
        ui->ErrorLabel->setText("Пароль должен быть не менее 8 символов и содержать буквы латинского алфавита в верхнем, нижнем регистрах и цифры.");
        ui->ErrorLabel->setVisible(true);
        return;
    }

    if (!nameChanged && pass.isEmpty()) {
        accept();
        return;
    }

    QString passhash = pass;
    if (!pass.isEmpty()) {
        passhash = QString::fromStdString(BCrypt::generateHash(pass.toStdString()));
    }

    ui->SaveButton->setEnabled(false);
    DataManager::instance().RequestUpdateUser(UserLogin, name, passhash);
}

void AccountEdit::OnUserUpdateFinished(bool success, const QString& login, const QString& name)
{
    if (login != UserLogin) {
        return;
    }
    ui->SaveButton->setEnabled(true);
    if (!success) {
        return;
    }
    emit UserEdited(UserLogin, name);
    accept();
}

void AccountEdit::on_CancelButton_clicked()
{
    this->close();
}

bool AccountEdit::IsPasswordValid(const QString& Password)
{
    const QRegularExpression pattern(R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,}$)");
    return pattern.match(Password).hasMatch();
}

bool AccountEdit::IsNameValid(const QString& Name)
{
    static const QRegularExpression pattern(R"(^[\p{Latin}\p{Cyrillic}]{1,45}$)",
                                            QRegularExpression::UseUnicodePropertiesOption);
    return pattern.match(Name).hasMatch();
}
