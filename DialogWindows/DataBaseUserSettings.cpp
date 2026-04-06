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

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    SetupDBStatus();

    ui->ErrorLabel->setVisible(false);
    
    ui->LoginEdit->setMaxLength(64);
    ui->PassEdit->setMaxLength(128);
    ui->PassEdit->setEchoMode(QLineEdit::Password);
}

DataBaseUserSettings::~DataBaseUserSettings()
{
    delete ui;
}

void DataBaseUserSettings::on_ConnectButton_clicked()
{
    emit ConnectRequested(ui->LoginEdit->text(), ui->PassEdit->text());
    accept();
}

void DataBaseUserSettings::SetupDBStatus()
{
    QSettings settings(AppConfig::GetDatabaseSettingsPath(), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("DataBase"));
        ui->LoginEdit->setText(settings.value(QStringLiteral("User")).toString().trimmed());
        ui->PassEdit->setText(settings.value(QStringLiteral("Password")).toString());
    settings.endGroup();
}

void DataBaseUserSettings::on_CloseButton_clicked()
{
    this->close();
}