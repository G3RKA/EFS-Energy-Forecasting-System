#include "AuditSettings.h"
#include "UI/ui_AuditSettings.h"

#include "AppConfig.h"

#include <QCheckBox>
#include <QSettings>

AuditSettings::AuditSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AuditSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("Audit");
        LoadAuditCheckbox(ui->DisableButton, settings, "AnalystDisableForecast");
        LoadAuditCheckbox(ui->EnableButton, settings, "AnalystEnableForecast");
        LoadAuditCheckbox(ui->UploadButton, settings, "AnalystUploadData");
        LoadAuditCheckbox(ui->ChangeButton, settings, "AnalystChangeData");
        LoadAuditCheckbox(ui->JoinButton, settings, "AnalystLoginLogout");
        LoadAuditCheckbox(ui->JoinButton_2, settings, "EnergyLoginLogout");
    settings.endGroup();

    connect(ui->DisableButton, &QCheckBox::toggled, this, [this](bool on) {
        WriteAuditFlag("AnalystDisableForecast", on);
    });
    connect(ui->EnableButton, &QCheckBox::toggled, this, [this](bool on) {
        WriteAuditFlag("AnalystEnableForecast", on);
    });
    connect(ui->UploadButton, &QCheckBox::toggled, this, [this](bool on) {
        WriteAuditFlag("AnalystUploadData", on);
    });
    connect(ui->ChangeButton, &QCheckBox::toggled, this, [this](bool on) {
        WriteAuditFlag("AnalystChangeData", on);
    });
    connect(ui->JoinButton, &QCheckBox::toggled, this, [this](bool on) {
        WriteAuditFlag("AnalystLoginLogout", on);
    });
    connect(ui->JoinButton_2, &QCheckBox::toggled, this, [this](bool on) {
        WriteAuditFlag("EnergyLoginLogout", on);
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

void AuditSettings::WriteAuditFlag(const QString& key, bool checked)
{
    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("Audit");
        settings.setValue(key, checked ? 1 : 0);
    settings.endGroup();
    settings.sync();
}

void AuditSettings::LoadAuditCheckbox(QCheckBox* box, QSettings& settings, const QString& key)
{
    box->blockSignals(true);
    box->setChecked(settings.value(key, 0).toInt() != 0);
    box->blockSignals(false);
}
