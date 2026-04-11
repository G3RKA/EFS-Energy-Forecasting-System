#include "DataMenu.h"
#include "UI/ui_DataMenu.h"

#include <QFileDialog>
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
    Q_UNUSED(path);
}

void DataMenu::on_CloseButton_clicked()
{
    this->close();
}
