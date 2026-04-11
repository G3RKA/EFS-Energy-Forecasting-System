#include "DataMenu.h"
#include "UI/ui_DataMenu.h"

#include "DataManager.h"
#include "UploadError.h"

#include <QAbstractButton>
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
    if (CSVWaitDialog) {
        delete CSVWaitDialog.data();
        CSVWaitDialog.clear();
    }
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

    ui->LoadButton->setEnabled(false);

    if (CSVWaitDialog) {
        delete CSVWaitDialog.data();
        CSVWaitDialog.clear();
    }

    auto *waitBox = new QMessageBox(QMessageBox::Information,
                                     "Загрузка CSV",
                                     "Обработка в процессе!",
                                     QMessageBox::Ok,
                                     this);
    waitBox->setWindowModality(Qt::WindowModal);
    SetupButton(waitBox);
    CSVWaitDialog = waitBox;

    waitBox->show();

    DataManager::instance().RequestProcessCSV(path);
}

void DataMenu::on_CloseButton_clicked()
{
    this->close();
}

void DataMenu::OnCSVProcessingFinished(bool success, const QString& message)
{
    if (CSVWaitDialog) {
        CSVWaitDialog->deleteLater();
        CSVWaitDialog.clear();
    }

    ui->LoadButton->setEnabled(true);

    if (success)
    {
        QMessageBox box(this);
        box.setWindowTitle("Загрузка CSV");
        box.setText("Обработка завершена!");
        box.setIcon(QMessageBox::Information);
        box.setStandardButtons(QMessageBox::Ok);
        SetupButton(&box);
        
        box.exec(); 
    }
    else
    {
        UploadError m(message, this);
        m.exec();
    }

}

void DataMenu::SetupButton(QMessageBox *Box)
{
    if (QAbstractButton *okBtn = Box->button(QMessageBox::Ok)) {
        okBtn->setStyleSheet("QPushButton { padding: 5px; }");
    }
}