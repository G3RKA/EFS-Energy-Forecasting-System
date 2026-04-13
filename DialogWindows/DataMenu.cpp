#include "DataMenu.h"
#include "UI/ui_DataMenu.h"

#include "AppConfig.h"
#include "DataManager.h"
#include "UploadError.h"

#include <QAbstractButton>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QFileDialog>
#include <QHeaderView>
#include <QLocale>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtMath>

DataMenu::DataMenu(const QString& AnalystLogin, QWidget *Parent)
    : QDialog(Parent)
    , ui(new Ui::DataMenu)
    , m_AnalystLogin(AnalystLogin)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->DataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->DataTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->DataTable->horizontalHeader()->setMinimumHeight(60);

    ui->DataPicker->setDate(QDate::currentDate());

    connect(&DataManager::instance(), &DataManager::CSVProcessingFinished,
            this, &DataMenu::OnCSVProcessingFinished);
    connect(&DataManager::instance(), &DataManager::ConsumerGroupsLoaded,
            this, &DataMenu::FillConsumePicker);
    connect(&DataManager::instance(), &DataManager::ConsumptionRowsLoaded,
            this, &DataMenu::FillDataTable);

    connect(ui->ConsumePicker, &QComboBox::currentIndexChanged, this, &DataMenu::OnGroupOrDateFilterChanged);
    connect(ui->DataPicker, &QDateEdit::dateChanged, this, &DataMenu::OnGroupOrDateFilterChanged);
    connect(ui->DataTable, &QTableWidget::itemChanged, this, &DataMenu::OnDataTableItemChanged);

    DataManager::instance().RequestConsumerGroupsList();
}

DataMenu::~DataMenu()
{
    if (CSVWaitDialog) {
        delete CSVWaitDialog.data();
        CSVWaitDialog.clear();
    }
    delete ui;
}

void DataMenu::on_LoadButton_clicked()
{
    const QString Path = QFileDialog::getOpenFileName(this,
                                                      "Выберите CSV-файл",
                                                      QString(),
                                                      "CSV (*.csv)");
    if (Path.isEmpty()) {
        return;
    }

    ui->LoadButton->setEnabled(false);

    if (CSVWaitDialog) {
        delete CSVWaitDialog.data();
        CSVWaitDialog.clear();
    }

    auto *WaitBox = new QMessageBox(QMessageBox::Information,
                                     "Загрузка CSV",
                                     "Обработка в процессе!",
                                     QMessageBox::Ok,
                                     this);
    WaitBox->setWindowModality(Qt::WindowModal);
    SetupButton(WaitBox);
    CSVWaitDialog = WaitBox;

    WaitBox->show();

    DataManager::instance().RequestProcessCSV(Path, m_AnalystLogin);
}

void DataMenu::on_CloseButton_clicked()
{
    this->close();
}

void DataMenu::OnCSVProcessingFinished(bool Success, const QString& Message)
{
    if (CSVWaitDialog) {
        CSVWaitDialog->deleteLater();
        CSVWaitDialog.clear();
    }

    ui->LoadButton->setEnabled(true);

    if (Success) {
        DataManager::instance().RequestConsumerGroupsList();

        QMessageBox Box(QMessageBox::Information,
                         "Загрузка CSV",
                         "Обработка завершена!",
                         QMessageBox::Ok,
                         this);
        SetupButton(&Box);

        Box.exec();
    }
    else
    {
        UploadError Dialog(Message, this);
        Dialog.exec();
    }

}

void DataMenu::SetupButton(QMessageBox *MessageBox)
{
    if (QAbstractButton *OkBtn = MessageBox->button(QMessageBox::Ok)) {
        OkBtn->setStyleSheet("QPushButton { padding: 5px; }");
    }
}

void DataMenu::FillConsumePicker(const QVector<ConsumerGroupRow>& Groups)
{
    ui->ConsumePicker->clear();
    for (const ConsumerGroupRow& Group : Groups) {
        ui->ConsumePicker->addItem(Group.Name, Group.GroupID);
    }

    ui->ConsumePicker->setCurrentIndex(0);

    RefreshConsumptionTable();
}

void DataMenu::RefreshConsumptionTable()
{
    const int GroupId = ui->ConsumePicker->currentData().toInt();
    if (GroupId <= 0) {
        ui->DataTable->setRowCount(0);
        return;
    }
    DataManager::instance().RequestConsumptionDataForGroupAndDate(GroupId, ui->DataPicker->date());
}

void DataMenu::OnGroupOrDateFilterChanged()
{
    RefreshConsumptionTable();
}

void DataMenu::FillDataTable(const QVector<ConsumerGroupConsumptionRow>& Rows)
{
    const QSignalBlocker Blocker(ui->DataTable);
    ui->DataTable->setRowCount(0);

    auto MakeReadOnlyCell = [](const QString& Text) {
        auto *Item = new QTableWidgetItem(Text);
        Item->setFlags((Item->flags() | Qt::ItemIsSelectable) & ~Qt::ItemIsEditable);
        Item->setTextAlignment(Qt::AlignCenter);
        return Item;
    };

    const QLocale Loc = QLocale::system();

    for (const ConsumerGroupConsumptionRow& Row : Rows) {
        const int R = ui->DataTable->rowCount();
        ui->DataTable->insertRow(R);

        ui->DataTable->setItem(R, 0, MakeReadOnlyCell(Row.ConsumerGroup));
        ui->DataTable->setItem(R, 1, MakeReadOnlyCell(Row.Date.toString("dd.MM.yyyy")));
        ui->DataTable->setItem(R, 2, MakeReadOnlyCell(QString::number(Row.Hour)));

        auto *CountItem = new QTableWidgetItem(QString::number(Row.ConsumerCount));
        CountItem->setTextAlignment(Qt::AlignCenter);
        CountItem->setFlags((CountItem->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable) & ~Qt::ItemIsUserCheckable);
        CountItem->setData(Qt::UserRole, Row.ConsumerCount);

        auto *ConsItem = new QTableWidgetItem(Loc.toString(Row.Consumption, 'f', 2));
        ConsItem->setTextAlignment(Qt::AlignCenter);
        ConsItem->setFlags((ConsItem->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable) & ~Qt::ItemIsUserCheckable);
        ConsItem->setData(Qt::UserRole, Row.Consumption);

        ui->DataTable->setItem(R, 3, CountItem);
        ui->DataTable->setItem(R, 4, ConsItem);
    }
}

void DataMenu::OnDataTableItemChanged(QTableWidgetItem *Item)
{
    if (!Item or (Item->column() != 3 && Item->column() != 4)) {
        return;
    }

    const int GroupId = ui->ConsumePicker->currentData().toInt();
    if (GroupId <= 0) {
        return;
    }

    QTableWidgetItem *HourItem = ui->DataTable->item(Item->row(), 2);
    QTableWidgetItem *CountItem = ui->DataTable->item(Item->row(), 3);
    QTableWidgetItem *ConsItem = ui->DataTable->item(Item->row(), 4);
    if (!HourItem or !CountItem or !ConsItem) {
        return;
    }

    const int Hour = HourItem->text().trimmed().toInt();

    const QDate Date = ui->DataPicker->date();
    const QLocale Loc = QLocale::system();

    bool CountOk = false;
    const int Count = CountItem->text().trimmed().toInt(&CountOk);
    bool ConsOk = false;
    const double Consumption = Loc.toDouble(ConsItem->text().trimmed(), &ConsOk);

    if (!CountOk or !ConsOk or Count < 0 or Consumption < 0) {
        const QSignalBlocker Blocker(ui->DataTable);
        CountItem->setText(QString::number(CountItem->data(Qt::UserRole).toInt()));
        ConsItem->setText(Loc.toString(ConsItem->data(Qt::UserRole).toDouble(), 'f', 2));
        return;
    }

    const int OldCount = CountItem->data(Qt::UserRole).toInt();
    const double OldConsumption = ConsItem->data(Qt::UserRole).toDouble();
    QTableWidgetItem *GroupItem = ui->DataTable->item(Item->row(), 0);
    const QString GroupName = GroupItem ? GroupItem->text() : ui->ConsumePicker->currentText();

    {
        const QSignalBlocker Blocker(ui->DataTable);
        CountItem->setText(QString::number(Count));
        CountItem->setData(Qt::UserRole, Count);
        ConsItem->setText(Loc.toString(Consumption, 'f', 2));
        ConsItem->setData(Qt::UserRole, Consumption);
    }

    if (AppConfig::IsAuditAnalystChangeDataEnabled() && !m_AnalystLogin.trimmed().isEmpty()) {
        const QString Login = m_AnalystLogin.trimmed();
        if (Item->column() == 3 && OldCount != Count) {
            const QString Msg = QString("Изменение количества потребителей для группы %1 с %2 на %3")
                                      .arg(GroupName)
                                      .arg(OldCount)
                                      .arg(Count);
            DataManager::instance().RequestAppendAuditLog(Login, Msg);
        }
        if (Item->column() == 4 && qAbs(OldConsumption - Consumption) > 1e-9) {
            const QString Msg = QString("Изменение потребления для группы %1 с %2 на %3")
                                    .arg(GroupName)
                                    .arg(Loc.toString(OldConsumption, 'f', 2))
                                    .arg(Loc.toString(Consumption, 'f', 2));
            DataManager::instance().RequestAppendAuditLog(Login, Msg);
        }
    }

    DataManager::instance().RequestUpdateConsumptionEntry(GroupId, Date, Hour, Count, Consumption);
}
