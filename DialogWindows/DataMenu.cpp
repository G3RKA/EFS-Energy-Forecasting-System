#include "DataMenu.h"
#include "UI/ui_DataMenu.h"

#include "DataManager.h"
#include "UploadError.h"

#include <QAbstractButton>
#include <QDate>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>

DataMenu::DataMenu(QWidget *Parent)
    : QDialog(Parent)
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

    DataManager::instance().RequestProcessCSV(Path);
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

    if (Success)
    {
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
