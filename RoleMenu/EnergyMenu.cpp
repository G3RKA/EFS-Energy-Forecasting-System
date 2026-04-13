#include "EnergyMenu.h"
#include "UI/ui_EnergyMenu.h"

#include "DataManager.h"

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

    connect(&DataManager::instance(), &DataManager::ForecastRowsLoaded, this, &EnergyMenu::FillForecastTable);

    DataManager::instance().RequestForecastDataForDateAndHour(QDate::currentDate(), QTime::currentTime().hour());
}

EnergyMenu::~EnergyMenu()
{
    delete ui;
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

    for (const ConsumerGroupConsumptionRow& Row : Rows) {
        const int R = ui->EnergyTable->rowCount();
        ui->EnergyTable->insertRow(R);
        ui->EnergyTable->setItem(R, 0, MakeCell(Row.ConsumerGroup));
        ui->EnergyTable->setItem(R, 1, MakeCell(QString::number(Row.ConsumerCount)));
        ui->EnergyTable->setItem(R, 2, MakeCell(Loc.toString(Row.Consumption, 'f', 2)));
    }
}
