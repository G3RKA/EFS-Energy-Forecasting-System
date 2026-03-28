#include "EnergyMenu.h"
#include "UI/ui_EnergyMenu.h"

EnergyMenu::EnergyMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EnergyMenu)
{
    ui->setupUi(this);

    ui->EnergyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->EnergyTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->EnergyTable->horizontalHeader()->setMinimumHeight(60);
}

EnergyMenu::~EnergyMenu()
{
    delete ui;
}
