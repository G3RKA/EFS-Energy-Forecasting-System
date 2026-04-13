#include "AnalystMenu.h"
#include "UI/ui_AnalystMenu.h"

#include "DataManager.h"
#include "DataMenu.h"
#include "ConsumersMenu.h"

#include <QDate>
#include <QHeaderView>
#include <QLocale>
#include <QSignalBlocker>
#include <QTableWidgetItem>
#include <QTime>

AnalystMenu::AnalystMenu(QWidget *Parent)
    : QWidget(Parent)
    , ui(new Ui::AnalystMenu)
{
    ui->setupUi(this);

    ui->EnergyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->EnergyTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->EnergyTable->horizontalHeader()->setMinimumHeight(60);

    connect(&DataManager::instance(), &DataManager::ForecastRowsLoaded, this, &AnalystMenu::FillForecastTable);

    DataManager::instance().RequestForecastDataForDateAndHour(QDate::currentDate(), QTime::currentTime().hour());
}

AnalystMenu::~AnalystMenu()
{
    delete ui;
}

void AnalystMenu::on_DataButton_clicked()
{
    DataMenu Menu(this);
    Menu.exec();
}

void AnalystMenu::on_ConsumeButton_clicked()
{
    ConsumersMenu Menu(this);
    Menu.exec();
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
        ui->EnergyTable->setItem(R, 2, MakeCell(Loc.toString(Row.Consumption, 'f', 2)));
    }
}
