#include "ConsumersMenu.h"
#include "UI/ui_ConsumersMenu.h"

ConsumersMenu::ConsumersMenu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConsumersMenu)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->ConsumeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ConsumeTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->ConsumeTable->horizontalHeader()->setMinimumHeight(60);
}

ConsumersMenu::~ConsumersMenu()
{
    delete ui;
}

void ConsumersMenu::on_ExitButton_clicked()
{
    this->close();
}

