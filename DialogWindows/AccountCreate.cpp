#include "AccountCreate.h"
#include "UI/ui_AccountCreate.h"

#include "DataManager.h"
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

    connect(&DataManager::instance(), &DataManager::UserCreateFinished, this, &AccountCreate::OnUserCreateFinished);
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
    if (!IsLoginValid(login)) {
        failed << "Логин должен состоять только из букв латинского алфавита и цифр";
    }
    if (!IsNameValid(name)) {
        failed << "Имя должно состоять только из букв русского или латинского алфавита";
    }
    if (!IsPasswordValid(pass)) {
        failed << "Пароль должен быть не менее 8 символов и содержать буквы латинского алфавита в верхнем, нижнем регистрах и цифры";
    }

    if (!failed.isEmpty()) {
        FailMessage = failed.join(".\n");
        FailMessage += '.';
        ui->ErrorLabel->setText(FailMessage);
        ui->ErrorLabel->setVisible(true);
        return;
    }

    const QString passhash = QString::fromStdString(BCrypt::generateHash(pass.toStdString()));

    ui->ErrorLabel->setVisible(false);
    PendingCreateLogin = login;
    ui->ConnectButton->setEnabled(false);
    DataManager::instance().RequestCreateUser(login, name, ui->RoleBox->currentText(), passhash);
}

void AccountCreate::OnUserCreateFinished(bool success, const QString& login, bool duplicateLogin)
{
    if (login != PendingCreateLogin) {
        return;
    }
    ui->ConnectButton->setEnabled(true);

    if (success) {
        emit tableUpdateRequested();
        accept();
        return;
    }

    if (duplicateLogin) {
        ui->ErrorLabel->setText("Логин должен быть уникальным.\n"+FailMessage);
        ui->ErrorLabel->setVisible(true);
        return;
    }
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
