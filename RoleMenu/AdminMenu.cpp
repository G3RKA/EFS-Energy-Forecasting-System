#include "AdminMenu.h"
#include "UI/ui_AdminMenu.h"

#include "AccountSettings.h"
#include "DataBaseSettings.h"
#include "AuditSettings.h"
#include "AuditMenu.h"

AdminMenu::AdminMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminMenu)
{
    ui->setupUi(this);

    ui->AuditTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AuditTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->AuditTable->horizontalHeader()->setMinimumHeight(60);

    QSizePolicy sp = ui->AuditFrame->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui->AuditFrame->setSizePolicy(sp);
    ui->AuditFrame->hide();

}

AdminMenu::~AdminMenu()
{
    delete ui;
}

void AdminMenu::on_AuditButton_clicked()
{
    ui->AuditFrame->setVisible(!ui->AuditFrame->isVisible());
}

void AdminMenu::on_DataBaseButton_clicked()
{
    DataBaseSettings Menu(this);
    Menu.exec();
}

void AdminMenu::on_AccountsButton_clicked()
{
    AccountSettings Menu(this);
    Menu.exec();
}

void AdminMenu::on_AuditSetButton_clicked()
{
    AuditSettings Menu(this);
    Menu.exec();
}

void AdminMenu::on_MoreButton_clicked()
{
    AuditMenu Menu(this);
    Menu.exec();
}

