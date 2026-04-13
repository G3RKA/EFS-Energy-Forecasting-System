#include "AdminMenu.h"
#include "UI/ui_AdminMenu.h"

#include "AccountSettings.h"
#include "AuditSettings.h"
#include "AuditMenu.h"
#include "DataBaseSettings.h"
#include "DataManager.h"

#include <QAbstractItemView>
#include <QDateTime>
#include <QHeaderView>
#include <QSizePolicy>
#include <QTableWidgetItem>

AdminMenu::AdminMenu(QWidget *Parent)
    : QWidget(Parent)
    , ui(new Ui::AdminMenu)
{
    ui->setupUi(this);

    ui->AuditTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AuditTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->AuditTable->horizontalHeader()->setMinimumHeight(60);
    ui->AuditTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->AuditTable->setWordWrap(true);
    ui->AuditTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(&DataManager::instance(), &DataManager::AuditLogAllLoaded, this, &AdminMenu::OnAuditLogAllLoaded);

    QSizePolicy Sp = ui->AuditFrame->sizePolicy();
    Sp.setRetainSizeWhenHidden(true);
    ui->AuditFrame->setSizePolicy(Sp);
    ui->AuditFrame->hide();

}

AdminMenu::~AdminMenu()
{
    delete ui;
}

void AdminMenu::on_AuditButton_clicked()
{
    const bool WillShow = !ui->AuditFrame->isVisible();
    ui->AuditFrame->setVisible(WillShow);
    if (WillShow) {
        DataManager::instance().RequestAuditLog();
    }
}

void AdminMenu::OnAuditLogAllLoaded(const QVector<AuditActionRow>& Rows)
{
    const int n = qMin(7, Rows.size());
    ui->AuditTable->setRowCount(n);

    auto makeCell = [](const QString& text) {
        auto* Item = new QTableWidgetItem(text);
        Item->setFlags(Item->flags() & ~Qt::ItemIsEditable);
        Item->setTextAlignment(Qt::Alignment(Qt::AlignCenter | Qt::TextWordWrap));
        return Item;
    };

    for (int i = 0; i < n; ++i) {
        const AuditActionRow& r = Rows.at(i);

        ui->AuditTable->setItem(i, 0, makeCell(r.UserName));
        ui->AuditTable->setItem(i, 1, makeCell(r.ActionDescription));

        QString DateTimeText;
        if (r.ActionDate.isValid()) {
            const QDateTime Dt(r.ActionDate, r.ActionTime);
            DateTimeText = Dt.toString("dd.MM.yyyy HH:mm");
        }
        ui->AuditTable->setItem(i, 2, makeCell(DateTimeText));
    }
    ui->AuditTable->resizeRowsToContents();
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

