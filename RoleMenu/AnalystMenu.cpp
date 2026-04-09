#include "AnalystMenu.h"
#include "UI/ui_AnalystMenu.h"

#include "DataMenu.h"
#include "ConsumersMenu.h"

AnalystMenu::AnalystMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AnalystMenu)
{
    ui->setupUi(this);

    ui->EnergyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->EnergyTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->EnergyTable->horizontalHeader()->setMinimumHeight(60);
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

