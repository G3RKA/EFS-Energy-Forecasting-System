#include "DataMenu.h"
#include "UI/ui_DataMenu.h"

#include "DataManager.h"

#include <QFileDialog>
#include <QMessageBox>

DataMenu::DataMenu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DataMenu)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->DataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->DataTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->DataTable->horizontalHeader()->setMinimumHeight(60);

    ui->DataPicker->setDate(QDate::currentDate());

    connect(&DataManager::instance(), &DataManager::CSVProcessingFinished,
            this, &DataMenu::OnCSVProcessingFinished);
}

DataMenu::~DataMenu()
{
    delete ui;
}

void DataMenu::on_LoadButton_clicked()
{
    const QString path = QFileDialog::getOpenFileName(this,
                                                      "Выберите CSV-файл",
                                                      QString(),
                                                      "CSV (*.csv)");
    if (path.isEmpty()) {
        return;
    }

    DataManager::instance().RequestProcessCSV(path);
}

void DataMenu::on_CloseButton_clicked()
{
    this->close();
}

void DataMenu::OnCSVProcessingFinished(bool success, const QString& message)
{
    const QString title = QStringLiteral("Загрузка CSV");
    if (success) {
        QMessageBox::information(this, title,
                                 message.isEmpty() ? QStringLiteral("Обработка завершена.") : message);
    } else {
        QMessageBox::warning(this, title,
                               message.isEmpty() ? QStringLiteral("Ошибка обработки файла.") : message);
    }
}
