#include "AuditSettings.h"
#include "UI/ui_AuditSettings.h"

#include "AppConfig.h"

#include <QCheckBox>
#include <QSettings>

AuditSettings::AuditSettings(QWidget *Parent)
    : QDialog(Parent)
    , ui(new Ui::AuditSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup(AppConfig::AuditGroup);
        LoadAuditCheckbox(ui->DisableButton, Settings, AppConfig::AuditKeyAnalystDisableForecast);
        LoadAuditCheckbox(ui->EnableButton, Settings, AppConfig::AuditKeyAnalystEnableForecast);
        LoadAuditCheckbox(ui->UploadButton, Settings, AppConfig::AuditKeyAnalystUploadData);
        LoadAuditCheckbox(ui->ChangeButton, Settings, AppConfig::AuditKeyAnalystChangeData);
        LoadAuditCheckbox(ui->JoinButton, Settings, AppConfig::AuditKeyAnalystLoginLogout);
        LoadAuditCheckbox(ui->JoinButton_2, Settings, AppConfig::AuditKeyEnergyLoginLogout);
    Settings.endGroup();

    connect(ui->DisableButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag(AppConfig::AuditKeyAnalystDisableForecast, On);
    });
    connect(ui->EnableButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag(AppConfig::AuditKeyAnalystEnableForecast, On);
    });
    connect(ui->UploadButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag(AppConfig::AuditKeyAnalystUploadData, On);
    });
    connect(ui->ChangeButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag(AppConfig::AuditKeyAnalystChangeData, On);
    });
    connect(ui->JoinButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag(AppConfig::AuditKeyAnalystLoginLogout, On);
    });
    connect(ui->JoinButton_2, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag(AppConfig::AuditKeyEnergyLoginLogout, On);
    });
}

AuditSettings::~AuditSettings()
{
    delete ui;
}

void AuditSettings::on_CloseButton_clicked()
{
    this->close();
}

void AuditSettings::WriteAuditFlag(const QString& Key, bool Checked)
{
    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup(AppConfig::AuditGroup);
        Settings.setValue(Key, Checked ? 1 : 0);
    Settings.endGroup();
    Settings.sync();
}

void AuditSettings::LoadAuditCheckbox(QCheckBox* Box, QSettings& Settings, const QString& Key)
{
    Box->blockSignals(true);
    Box->setChecked(Settings.value(Key, 0).toInt() != 0);
    Box->blockSignals(false);
}
