#include "AccountEdit.h"
#include "UI/ui_AccountEdit.h"

#include "DataManager.h"

#include <QLineEdit>
#include <QRegularExpression>

#include "bcrypt.hpp"

AccountEdit::AccountEdit(QWidget *Parent, const QString& InLogin, const QString& InName)
    : QDialog(Parent)
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

    const QString Name = ui->NameEdit->text().trimmed();
    const QString Pass = ui->PassEdit->text().trimmed();
    const bool NameChanged = (Name != UserOldName);

    if (NameChanged and !IsNameValid(Name)) {
        ui->ErrorLabel->setText("Имя должно состоять только из букв русского или латинского алфавита.");
        ui->ErrorLabel->setVisible(true);
        return;
    }

    if (!Pass.isEmpty() and !IsPasswordValid(Pass)) {
        ui->ErrorLabel->setText("Пароль должен быть не менее 8 символов и содержать буквы латинского алфавита в верхнем, нижнем регистрах и цифры.");
        ui->ErrorLabel->setVisible(true);
        return;
    }

    if (!NameChanged && Pass.isEmpty()) {
        accept();
        return;
    }

    QString PassHash = Pass;
    if (!Pass.isEmpty()) {
        PassHash = QString::fromStdString(BCrypt::generateHash(Pass.toStdString()));
    }

    ui->SaveButton->setEnabled(false);
    DataManager::instance().RequestUpdateUser(UserLogin, Name, PassHash);
}

void AccountEdit::OnUserUpdateFinished(bool Success, const QString& Login, const QString& Name)
{
    if (Login != UserLogin) {
        return;
    }
    ui->SaveButton->setEnabled(true);
    if (!Success) {
        return;
    }
    emit UserEdited(UserLogin, Name);
    accept();
}

void AccountEdit::on_CancelButton_clicked()
{
    this->close();
}

bool AccountEdit::IsPasswordValid(const QString& Password)
{
    const QRegularExpression Pattern(R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,}$)");
    return Pattern.match(Password).hasMatch();
}

bool AccountEdit::IsNameValid(const QString& Name)
{
    static const QRegularExpression Pattern(R"(^[\p{Latin}\p{Cyrillic}]{1,45}$)",
                                            QRegularExpression::UseUnicodePropertiesOption);
    return Pattern.match(Name).hasMatch();
}
