#include "DataBaseUserSettings.h"
#include "AppConfig.h"
#include "UI/ui_DataBaseUserSettings.h"

#include <QLineEdit>
#include <QSettings>

DataBaseUserSettings::DataBaseUserSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DataBaseUserSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    SetupDBStatus();

    ui->ErrorLabel->setVisible(false);
    
    ui->LoginEdit->setMaxLength(64);
    ui->PassEdit->setMaxLength(128);
    ui->PassEdit->setEchoMode(QLineEdit::Password);

    connect(ui->LoginEdit, &QLineEdit::textEdited, this, &DataBaseUserSettings::ConnectionDataChanged);
    connect(ui->PassEdit, &QLineEdit::textEdited, this, &DataBaseUserSettings::ConnectionDataChanged);
     
    ConnectionDataChanged();
}

DataBaseUserSettings::~DataBaseUserSettings()
{
    delete ui;
}

void DataBaseUserSettings::OnCloseUserConnection()
{
    accept();
}

void DataBaseUserSettings::OnLoginError()
{
    ui->ErrorLabel->setVisible(true);
    ui->ErrorLabel->setText("Неверный логин или пароль");
}

void DataBaseUserSettings::on_ConnectButton_clicked()
{
    emit ConnectRequested(ui->LoginEdit->text(), ui->PassEdit->text());
}

void DataBaseUserSettings::on_CloseButton_clicked()
{
    this->close();
}

void DataBaseUserSettings::ConnectionDataChanged()
{
    ui->ConnectButton->setEnabled(not(ui->LoginEdit->text().isEmpty()) and not(ui->PassEdit->text().isEmpty()));
}

void DataBaseUserSettings::SetupDBStatus()
{
    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("DataBase");
        ui->LoginEdit->setText(settings.value("User").toString().trimmed());
        ui->PassEdit->setText(settings.value("Password").toString());
    settings.endGroup();
}