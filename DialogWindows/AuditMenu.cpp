#include "AuditMenu.h"
#include "UI/ui_AuditMenu.h"

#include "DataManager.h"

#include <QAbstractItemView>
#include <QDate>
#include <QDateEdit>
#include <QHeaderView>
#include <QTableWidgetItem>

AuditMenu::AuditMenu(QWidget *Parent)
    : QDialog(Parent)
    , ui(new Ui::AuditMenu)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->AuditTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AuditTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->AuditTable->horizontalHeader()->setMinimumHeight(60);
    ui->AuditTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->AuditTable->setWordWrap(true);
    ui->AuditTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(&DataManager::instance(), &DataManager::AuditLogForDayLoaded, this, &AuditMenu::OnAuditLogForDayLoaded);
    connect(ui->DataPicker, &QDateEdit::dateChanged, &DataManager::instance(), &DataManager::RequestAuditLog);

    ui->DataPicker->setDate(QDate::currentDate());

    DataManager::instance().RequestAuditLog(ui->DataPicker->date());
}

AuditMenu::~AuditMenu()
{
    delete ui;
}

void AuditMenu::on_CloseButton_clicked()
{
    this->close();
}

void AuditMenu::OnAuditLogForDayLoaded(const QVector<AuditActionRow>& Rows)
{
    FillAuditTable(Rows);
}

void AuditMenu::FillAuditTable(const QVector<AuditActionRow>& Rows)
{
    const int n = Rows.size();
    ui->AuditTable->setRowCount(n);

    const auto makeCell = [](const QString& text) {
        auto* Item = new QTableWidgetItem(text);
        Item->setFlags(Item->flags() & ~Qt::ItemIsEditable);
        Item->setTextAlignment(Qt::Alignment(Qt::AlignCenter | Qt::TextWordWrap));
        return Item;
    };

    for (int i = 0; i < n; ++i) {
        const AuditActionRow& r = Rows.at(i);
        ui->AuditTable->setItem(i, 0, makeCell(r.UserRole));
        ui->AuditTable->setItem(i, 1, makeCell(r.UserName));
        ui->AuditTable->setItem(i, 2, makeCell(r.UserLogin));
        ui->AuditTable->setItem(i, 3, makeCell(r.ActionDescription));

        QString DateText;
        if (r.ActionDate.isValid()) {
            DateText = r.ActionDate.toString(QStringLiteral("dd.MM.yyyy"));
        }
        ui->AuditTable->setItem(i, 4, makeCell(DateText));

        QString TimeText;
        if (r.ActionTime.isValid()) {
            TimeText = r.ActionTime.toString(QStringLiteral("HH:mm"));
        }
        ui->AuditTable->setItem(i, 5, makeCell(TimeText));
    }
    ui->AuditTable->resizeRowsToContents();
}
