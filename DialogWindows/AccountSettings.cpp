#include "AccountSettings.h"
#include "UI/ui_AccountSettings.h"

#include "AccountCreate.h"

AccountSettings::AccountSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccountSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->AccountTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AccountTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->AccountTable->horizontalHeader()->setMinimumHeight(60);
}

AccountSettings::~AccountSettings()
{
    delete ui;
}

void AccountSettings::on_ExitButton_clicked()
{
    this->close();
}


void AccountSettings::on_CreateButton_clicked()
{
    AccountCreate Menu;
    Menu.exec();
}

