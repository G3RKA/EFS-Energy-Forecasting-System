#include "AccountCreate.h"
#include "UI/ui_AccountCreate.h"

#include "DataManager.h"
#include "bcrypt.hpp"

#include <QRegularExpression>

AccountCreate::AccountCreate(QWidget *Parent)
    : QDialog(Parent)
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
    const QString Login = ui->LoginEdit->text().trimmed();
    const QString Name = ui->NameEdit->text().trimmed();
    const QString Pass = ui->PassEdit->text().trimmed();

    QStringList Failed;
    if (!IsLoginValid(Login)) {
        Failed << "Логин должен состоять только из букв латинского алфавита и цифр";
    }
    if (!IsNameValid(Name)) {
        Failed << "Имя должно состоять только из букв русского или латинского алфавита";
    }
    if (!IsPasswordValid(Pass)) {
        Failed << "Пароль должен быть не менее 8 символов и содержать буквы латинского алфавита в верхнем, нижнем регистрах и цифры";
    }

    if (!Failed.isEmpty()) {
        FailMessage = Failed.join(".\n");
        FailMessage += '.';
        ui->ErrorLabel->setText(FailMessage);
        ui->ErrorLabel->setVisible(true);
        return;
    }

    const QString PassHash = QString::fromStdString(BCrypt::generateHash(Pass.toStdString()));

    ui->ErrorLabel->setVisible(false);
    PendingCreateLogin = Login;
    ui->ConnectButton->setEnabled(false);
    DataManager::instance().RequestCreateUser(Login, Name, ui->RoleBox->currentText(), PassHash);
}

void AccountCreate::OnUserCreateFinished(bool Success, const QString& Login, bool DuplicateLogin)
{
    if (Login != PendingCreateLogin) {
        return;
    }
    ui->ConnectButton->setEnabled(true);

    if (Success) {
        emit tableUpdateRequested();
        accept();
        return;
    }

    if (DuplicateLogin) {
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
    const QRegularExpression Pattern(R"(^[A-Za-z0-9]+$)");
    return Pattern.match(Login).hasMatch();
}

bool AccountCreate::IsPasswordValid(const QString& Password)
{
    const QRegularExpression Pattern(R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,}$)");
    return Pattern.match(Password).hasMatch();
}

bool AccountCreate::IsNameValid(const QString& Name)
{
    static const QRegularExpression Pattern(R"(^[\p{Latin}\p{Cyrillic}]{1,45}$)",
                                            QRegularExpression::UseUnicodePropertiesOption);
    return Pattern.match(Name).hasMatch();
}
