#include "ConsumersMenu.h"
#include "UI/ui_ConsumersMenu.h"

#include "DataManager.h"

#include <QHeaderView>
#include <QSignalBlocker>
#include <QTableWidget>
#include <QTableWidgetItem>

ConsumersMenu::ConsumersMenu(QWidget *Parent)
    : QDialog(Parent)
    , ui(new Ui::ConsumersMenu)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->ConsumeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ConsumeTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->ConsumeTable->horizontalHeader()->setMinimumHeight(60);
    ui->ConsumeTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(&DataManager::instance(), &DataManager::ConsumerGroupsLoaded, this, &ConsumersMenu::LoadConsumerGroups);
    connect(ui->ConsumeTable, &QTableWidget::itemChanged, this, &ConsumersMenu::OnConsumeTableItemChanged);

    DataManager::instance().RequestConsumerGroupsList();
}

ConsumersMenu::~ConsumersMenu()
{
    delete ui;
}

void ConsumersMenu::on_ExitButton_clicked()
{
    this->close();
}

void ConsumersMenu::LoadConsumerGroups(const QVector<ConsumerGroupRow>& Groups)
{
    const QSignalBlocker Blocker(ui->ConsumeTable);
    ui->ConsumeTable->setRowCount(0);

    for (const ConsumerGroupRow& Group : Groups) {
        const int Row = ui->ConsumeTable->rowCount();
        ui->ConsumeTable->insertRow(Row);

        auto *NameItem = new QTableWidgetItem(Group.Name);
        NameItem->setFlags((NameItem->flags() | Qt::ItemIsSelectable) & ~Qt::ItemIsEditable);
        NameItem->setTextAlignment(Qt::AlignCenter);

        auto *AmountItem = new QTableWidgetItem(QString::number(Group.Amount));
        AmountItem->setTextAlignment(Qt::AlignCenter);
        AmountItem->setData(Qt::UserRole, Group.Amount);

        ui->ConsumeTable->setItem(Row, 0, NameItem);
        ui->ConsumeTable->setItem(Row, 1, AmountItem);
    }

    ui->ConsumeTable->resizeRowsToContents();
}

void ConsumersMenu::OnConsumeTableItemChanged(QTableWidgetItem *Item)
{
    if (!Item or Item->column() != 1) {
        return;
    }

    bool ParseOk = false;
    const int Amount = Item->text().trimmed().toInt(&ParseOk);
    if (!ParseOk or Amount < 0) {
        bool PrevOk = false;
        const int Previous = Item->data(Qt::UserRole).toInt(&PrevOk);
        const QSignalBlocker Blocker(ui->ConsumeTable);
        Item->setText(QString::number(Previous));
        return;
    }

    Item->setData(Qt::UserRole, Amount);

    QTableWidgetItem *NameItem = ui->ConsumeTable->item(Item->row(), 0);
    if (!NameItem) {
        return;
    }

    ConsumerAmountChanged(NameItem->text(), Amount);
}

void ConsumersMenu::ConsumerAmountChanged(const QString& GroupName, int Amount)
{
    DataManager::instance().RequestUpdateConsumerGroupAmount(GroupName, Amount);
}

