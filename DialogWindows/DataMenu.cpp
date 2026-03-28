#include "DataMenu.h"
#include "UI/ui_DataMenu.h"

#include "UploadError.h"

DataMenu::DataMenu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DataMenu)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

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
    UploadError Menu;
    Menu.exec();
}

void DataMenu::on_CloseButton_clicked()
{
    this->close();
}
