#include "DataBaseSettings.h"
#include "AppConfig.h"
#include "UI/ui_DataBaseSettings.h"

#include "DataBaseUserSettings.h"
#include "DataManager.h"

#include <QLineEdit>
#include <QIntValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>
#include <QSettings>

DataBaseSettings::DataBaseSettings(QWidget *Parent)
    : QDialog(Parent)
    , ui(new Ui::DataBaseSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    SetupDBStatus();
    DataManager::instance().RequestDBState();
    SetupInputConstraints();

    ui->ErrorLabel->setVisible(false);

    ui->ConnectButton->setDisabled(true);

    connect(&DataManager::instance(), &DataManager::OnDataBaseStateSended, this, &DataBaseSettings::UpdateDBState);
    connect(&DataManager::instance(), &DataManager::SettingsAnonymousResult, this,
            &DataBaseSettings::OnSettingsAnonymousResult);
    connect(&DataManager::instance(), &DataManager::SettingsUserResult, this, &DataBaseSettings::OnSettingsUserResult);

    connect(ui->HostEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    connect(ui->PortEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    connect(ui->NameEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);

    ConnectionDataChanged();
}

DataBaseSettings::~DataBaseSettings()
{
    delete ui;
}

void DataBaseSettings::on_CloseButton_clicked()
{
    this->close();
}

void DataBaseSettings::on_ConnectButton_clicked()
{
    ui->ConnectButton->setDisabled(true);
    SaveDatabaseConnectionSettings();
    DataManager::instance().RequestTrySettingsAnonymous();
}

void DataBaseSettings::UpdateDBState(bool Connected)
{
    ui->StatusInfoLabel->setText(Connected ? "ПОДКЛЮЧЕНО" : "ОТКЛЮЧЕНО");
}

void DataBaseSettings::OnSettingsAnonymousResult(bool OpenUserDialog, bool ShowErrorLabel, const QString& ErrorLabelText)
{
    if (ShowErrorLabel) {
        ui->ErrorLabel->setVisible(true);
        ui->ErrorLabel->setText(ErrorLabelText);
    } else {
        ui->ErrorLabel->setVisible(false);
    }

    if (OpenUserDialog) {
        DataBaseUserSettings Menu;
        connect(&Menu, &DataBaseUserSettings::ConnectRequested, this, &DataBaseSettings::OnDatabaseUserSubmitted);
        connect(this, &DataBaseSettings::CloseUserConnection, &Menu, &DataBaseUserSettings::OnCloseUserConnection);
        connect(this, &DataBaseSettings::LoginError, &Menu, &DataBaseUserSettings::OnLoginError);
        Menu.exec();
    }

    DataManager::instance().RequestDBState();
    ui->ConnectButton->setDisabled(false);
}

void DataBaseSettings::OnSettingsUserResult(bool Success, bool UnknownDatabase, bool BadCredentials,
                                            const QString& UnknownDbMessage)
{
    DataManager::instance().RequestDBState();

    if (Success) {
        ui->ErrorLabel->setVisible(false);
        emit CloseUserConnection();
        return;
    }

    if (UnknownDatabase) {
        ui->ErrorLabel->setVisible(true);
        ui->ErrorLabel->setText(UnknownDbMessage);
        emit CloseUserConnection();
        return;
    }

    if (BadCredentials) {
        emit LoginError();
    }
}

void DataBaseSettings::ConnectionDataChanged()
{
    auto LineAcceptable = [](QLineEdit *Edit) {
        if (Edit->text().isEmpty())
            return false;
        int Pos = 0;
        QString T = Edit->text();
        return Edit->validator()->validate(T, Pos) == QValidator::Acceptable;
    };

    ui->ConnectButton->setEnabled(LineAcceptable(ui->HostEdit) and LineAcceptable(ui->PortEdit)
                                  and LineAcceptable(ui->NameEdit));
}

void DataBaseSettings::OnDatabaseUserSubmitted(const QString& User, const QString& Password)
{
    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup(AppConfig::DatabaseGroup);
        Settings.setValue(AppConfig::DatabaseLoginKey, User.trimmed());
        Settings.setValue(AppConfig::DatabasePasswordKey, Password.trimmed());
    Settings.endGroup();
    Settings.sync();

    DataManager::instance().RequestTrySettingsWithUser();
}

void DataBaseSettings::SetupDBStatus()
{
    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup(AppConfig::DatabaseGroup);
        ui->HostEdit->setText(Settings.value(AppConfig::DatabaseHostKey).toString().trimmed());
        ui->PortEdit->setText(Settings.value(AppConfig::DatabasePortKey).toString().trimmed());
        ui->NameEdit->setText(Settings.value(AppConfig::DatabaseNameKey).toString().trimmed());
    Settings.endGroup();
}

void DataBaseSettings::SetupInputConstraints()
{
    ui->HostEdit->setMaxLength(255);
    const QRegularExpression HostChars(R"(^[a-zA-Z0-9._]*$)");
    ui->HostEdit->setValidator(new QRegularExpressionValidator(HostChars, ui->HostEdit));

    ui->PortEdit->setMaxLength(5);
    ui->PortEdit->setValidator(new QIntValidator(1, 65535, ui->PortEdit));

    ui->NameEdit->setMaxLength(63);
    const QRegularExpression DbName(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
    ui->NameEdit->setValidator(new QRegularExpressionValidator(DbName, ui->NameEdit));
}

void DataBaseSettings::SaveDatabaseConnectionSettings()
{
    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup(AppConfig::DatabaseGroup);
        Settings.setValue(AppConfig::DatabaseHostKey , ui->HostEdit->text());
        Settings.setValue(AppConfig::DatabasePortKey, ui->PortEdit->text());
        Settings.setValue(AppConfig::DatabaseNameKey, ui->NameEdit->text());
    Settings.endGroup();
    Settings.sync();
}
