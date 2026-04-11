#include "AuditMenu.h"
#include "UI/ui_AuditMenu.h"

#include <QDate>
#include <QHeaderView>

AuditMenu::AuditMenu(QWidget *Parent)
    : QDialog(Parent)
    , ui(new Ui::AuditMenu)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->AuditTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AuditTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->AuditTable->horizontalHeader()->setMinimumHeight(60);

    ui->DataPicker->setDate(QDate::currentDate());
}

AuditMenu::~AuditMenu()
{
    delete ui;
}

void AuditMenu::on_CloseButton_clicked()
{
    this->close();
}

