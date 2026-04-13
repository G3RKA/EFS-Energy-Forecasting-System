#include "AnalystMenu.h"
#include "UI/ui_AnalystMenu.h"

#include "AppConfig.h"
#include "DataManager.h"
#include "ForecastManager.h"
#include "DataMenu.h"
#include "ConsumersMenu.h"

#include <QDate>
#include <QHeaderView>
#include <QLocale>
#include <QSettings>
#include <QSignalBlocker>
#include <QTableWidgetItem>
#include <QTime>

AnalystMenu::AnalystMenu(const QString& AnalystLogin, QWidget *Parent)
    : QWidget(Parent)
    , ui(new Ui::AnalystMenu)
    , m_AnalystLogin(AnalystLogin)
{
    ui->setupUi(this);

    ui->EnergyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->EnergyTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->EnergyTable->horizontalHeader()->setMinimumHeight(60);

    RefreshForecastStatusLabel();

    connect(&DataManager::instance(), &DataManager::ForecastRowsLoaded, this, &AnalystMenu::FillForecastTable);
    connect(&ForecastManager::instance(), &ForecastManager::RemainingTimeToNextFullHour, this,
            &AnalystMenu::OnRemainingTimeToNextFullHour);

    DataManager::instance().RequestForecastDataForDateAndHour(QDate::currentDate(), QTime::currentTime().hour());
}

AnalystMenu::~AnalystMenu()
{
    delete ui;
}

void AnalystMenu::RefreshForecastStatusLabel()
{
    const bool ForecastOn = AppConfig::IsForecastEnabled();
    ui->StatusInfo->setText(AppConfig::ForecastStatusCaption());
    ui->TurnOnButton->setEnabled(!ForecastOn);
    ui->TurnOffButton->setEnabled(ForecastOn);
}

void AnalystMenu::WriteForecastStatusToIni(bool enabled)
{
    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup(AppConfig::ForecastGroup);
        Settings.setValue(AppConfig::ForecastStatusKey, enabled ? 1 : 0);
    Settings.endGroup();
    Settings.sync();
}

void AnalystMenu::on_DataButton_clicked()
{
    DataMenu Menu(m_AnalystLogin, this);
    Menu.exec();
}

void AnalystMenu::on_ConsumeButton_clicked()
{
    ConsumersMenu Menu(this);
    Menu.exec();
}

void AnalystMenu::OnRemainingTimeToNextFullHour(qint64 remainingMs)
{
    ui->TimeRemLabel->setText(ForecastManager::FormatRemainingTime(remainingMs));
}

void AnalystMenu::FillForecastTable(const QVector<ConsumerGroupConsumptionRow>& Rows)
{
    const QSignalBlocker Blocker(ui->EnergyTable);
    ui->EnergyTable->setRowCount(0);

    auto MakeCell = [](const QString& Text) {
        auto *Item = new QTableWidgetItem(Text);
        Item->setFlags((Item->flags() | Qt::ItemIsSelectable) & ~Qt::ItemIsEditable);
        Item->setTextAlignment(Qt::AlignCenter);
        return Item;
    };
    const QLocale Loc = QLocale::system();

    for (const ConsumerGroupConsumptionRow& Row : Rows) {
        const int R = ui->EnergyTable->rowCount();
        ui->EnergyTable->insertRow(R);
        ui->EnergyTable->setItem(R, 0, MakeCell(Row.ConsumerGroup));
        ui->EnergyTable->setItem(R, 1, MakeCell(QString::number(Row.ConsumerCount)));
        if (Row.Consumption >= 0.0)
        {
            ui->EnergyTable->setItem(R, 2, MakeCell(Loc.toString(Row.Consumption, 'f', 2)));
        }
        else 
        {
            ui->EnergyTable->setItem(R, 2, MakeCell("Нет данных"));
        }
    }
}

void AnalystMenu::on_TurnOnButton_clicked()
{
    WriteForecastStatusToIni(true);
    ForecastManager::instance().SyncWorkerWithSettings();
    RefreshForecastStatusLabel();
    if (AppConfig::IsAuditAnalystEnableForecastEnabled() && !m_AnalystLogin.trimmed().isEmpty()) {
        DataManager::instance().RequestAppendAuditLog(m_AnalystLogin.trimmed(),
                                                      "Включено прогнозирование");
    }
}

void AnalystMenu::on_TurnOffButton_clicked()
{
    WriteForecastStatusToIni(false);
    ForecastManager::instance().SyncWorkerWithSettings();
    RefreshForecastStatusLabel();
    ui->TimeRemLabel->setText("-");
    if (AppConfig::IsAuditAnalystDisableForecastEnabled() && !m_AnalystLogin.trimmed().isEmpty()) {
        DataManager::instance().RequestAppendAuditLog(m_AnalystLogin.trimmed(),
                                                      "Отключено прогнозирование");
    }
}

