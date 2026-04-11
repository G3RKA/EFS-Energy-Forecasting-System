#include "DataBaseSettings.h"
#include "AppConfig.h"
#include "UI/ui_DataBaseSettings.h"

#include "DataBaseUserSettings.h"
#include "DataManager.h"

#include <QIntValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>
#include <QSettings>

DataBaseSettings::DataBaseSettings(QWidget *parent)
    : QDialog(parent)
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

void DataBaseSettings::UpdateDBState(bool connected)
{
    ui->StatusInfoLabel->setText(connected ? "ПОДКЛЮЧЕНО" : "ОТКЛЮЧЕНО");
}

void DataBaseSettings::OnSettingsAnonymousResult(bool openUserDialog, bool showErrorLabel, const QString& errorLabelText)
{
    if (showErrorLabel) {
        ui->ErrorLabel->setVisible(true);
        ui->ErrorLabel->setText(errorLabelText);
    } else {
        ui->ErrorLabel->setVisible(false);
    }

    if (openUserDialog) {
        DataBaseUserSettings menu;
        connect(&menu, &DataBaseUserSettings::ConnectRequested, this, &DataBaseSettings::OnDatabaseUserSubmitted);
        connect(this, &DataBaseSettings::CloseUserConnection, &menu, &DataBaseUserSettings::OnCloseUserConnection);
        connect(this, &DataBaseSettings::LoginError, &menu, &DataBaseUserSettings::OnLoginError);
        menu.exec();
    }

    DataManager::instance().RequestDBState();
    ui->ConnectButton->setDisabled(false);
}

void DataBaseSettings::OnSettingsUserResult(bool success, bool unknownDatabase, bool badCredentials,
                                            const QString& unknownDbMessage)
{
    DataManager::instance().RequestDBState();

    if (success) {
        ui->ErrorLabel->setVisible(false);
        emit CloseUserConnection();
        return;
    }

    if (unknownDatabase) {
        ui->ErrorLabel->setVisible(true);
        ui->ErrorLabel->setText(unknownDbMessage);
        emit CloseUserConnection();
        return;
    }

    if (badCredentials) {
        emit LoginError();
    }
}

void DataBaseSettings::ConnectionDataChanged()
{
    auto lineAcceptable = [](QLineEdit *edit) {
        if (edit->text().isEmpty())
            return false;
        int pos = 0;
        QString t = edit->text();
        return edit->validator()->validate(t, pos) == QValidator::Acceptable;
    };

    ui->ConnectButton->setEnabled(lineAcceptable(ui->HostEdit) and lineAcceptable(ui->PortEdit)
                                  and lineAcceptable(ui->NameEdit));
}

void DataBaseSettings::OnDatabaseUserSubmitted(const QString& user, const QString& password)
{
    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("DataBase");
        settings.setValue("User", user.trimmed());
        settings.setValue("Password", password.trimmed());
    settings.endGroup();
    settings.sync();

    DataManager::instance().RequestTrySettingsWithUser();
}

void DataBaseSettings::SetupDBStatus()
{
    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("DataBase");
        ui->HostEdit->setText(settings.value("Host").toString().trimmed());
        ui->PortEdit->setText(settings.value("Port").toString().trimmed());
        ui->NameEdit->setText(settings.value("Name").toString().trimmed());
    settings.endGroup();
}

void DataBaseSettings::SetupInputConstraints()
{
    ui->HostEdit->setMaxLength(255);
    const QRegularExpression hostChars(R"(^[a-zA-Z0-9._]*$)");
    ui->HostEdit->setValidator(new QRegularExpressionValidator(hostChars, ui->HostEdit));

    ui->PortEdit->setMaxLength(5);
    ui->PortEdit->setValidator(new QIntValidator(1, 65535, ui->PortEdit));

    ui->NameEdit->setMaxLength(63);
    const QRegularExpression dbName(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
    ui->NameEdit->setValidator(new QRegularExpressionValidator(dbName, ui->NameEdit));
}

void DataBaseSettings::SaveDatabaseConnectionSettings()
{
    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("DataBase");
    settings.setValue("Host", ui->HostEdit->text());
    settings.setValue("Port", ui->PortEdit->text());
    settings.setValue("Name", ui->NameEdit->text());
    settings.endGroup();
    settings.sync();
}
