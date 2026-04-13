#include "EnergyMenu.h"
#include "UI/ui_EnergyMenu.h"

#include "AppConfig.h"
#include "DataManager.h"
#include "ForecastManager.h"

#include <QDate>
#include <QHeaderView>
#include <QLocale>
#include <QSignalBlocker>
#include <QTableWidgetItem>
#include <QTime>

EnergyMenu::EnergyMenu(QWidget *Parent)
    : QWidget(Parent)
    , ui(new Ui::EnergyMenu)
{
    ui->setupUi(this);

    ui->EnergyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->EnergyTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->EnergyTable->horizontalHeader()->setMinimumHeight(60);

    RefreshForecastStatusLabel();

    connect(&DataManager::instance(), &DataManager::ForecastRowsLoaded, this, &EnergyMenu::FillForecastTable);
    connect(&ForecastManager::instance(), &ForecastManager::RemainingTimeToNextFullHour, this,
            &EnergyMenu::OnRemainingTimeToNextFullHour);
    connect(&ForecastManager::instance(), &ForecastManager::FullHourReached, this,
            &EnergyMenu::OnForecastFullHourReached);

    DataManager::instance().RequestForecastDataForDateAndHour(QDate::currentDate(), QTime::currentTime().hour());
}

EnergyMenu::~EnergyMenu()
{
    delete ui;
}

void EnergyMenu::RefreshForecastStatusLabel()
{
    ui->StatusInfo->setText(AppConfig::ForecastStatusCaption());
}

void EnergyMenu::OnRemainingTimeToNextFullHour(qint64 remainingMs)
{
    ui->TimeRemLabel->setText(ForecastManager::FormatRemainingTime(remainingMs));
}

void EnergyMenu::OnForecastFullHourReached()
{
    DataManager::instance().RequestForecastDataForDateAndHour(QDate::currentDate(), QTime::currentTime().hour());
}

void EnergyMenu::FillForecastTable(const QVector<ConsumerGroupConsumptionRow>& Rows)
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

    double TotalConsumption = 0;
    for (const ConsumerGroupConsumptionRow& Row : Rows) {
        const int R = ui->EnergyTable->rowCount();
        ui->EnergyTable->insertRow(R);
        ui->EnergyTable->setItem(R, 0, MakeCell(Row.ConsumerGroup));
        ui->EnergyTable->setItem(R, 1, MakeCell(QString::number(Row.ConsumerCount)));
        ui->EnergyTable->setItem(R, 2, MakeCell(Loc.toString(Row.Consumption, 'f', 2)));
        TotalConsumption += Row.Consumption;
    }
    ui->SummaryInfoText->setText(Loc.toString(TotalConsumption, 'f', 2));
}
