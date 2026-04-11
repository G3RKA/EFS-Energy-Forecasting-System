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
    Settings.beginGroup("Audit");
        LoadAuditCheckbox(ui->DisableButton, Settings, "AnalystDisableForecast");
        LoadAuditCheckbox(ui->EnableButton, Settings, "AnalystEnableForecast");
        LoadAuditCheckbox(ui->UploadButton, Settings, "AnalystUploadData");
        LoadAuditCheckbox(ui->ChangeButton, Settings, "AnalystChangeData");
        LoadAuditCheckbox(ui->JoinButton, Settings, "AnalystLoginLogout");
        LoadAuditCheckbox(ui->JoinButton_2, Settings, "EnergyLoginLogout");
    Settings.endGroup();

    connect(ui->DisableButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag("AnalystDisableForecast", On);
    });
    connect(ui->EnableButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag("AnalystEnableForecast", On);
    });
    connect(ui->UploadButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag("AnalystUploadData", On);
    });
    connect(ui->ChangeButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag("AnalystChangeData", On);
    });
    connect(ui->JoinButton, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag("AnalystLoginLogout", On);
    });
    connect(ui->JoinButton_2, &QCheckBox::toggled, this, [this](bool On) {
        WriteAuditFlag("EnergyLoginLogout", On);
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
    Settings.beginGroup("Audit");
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
