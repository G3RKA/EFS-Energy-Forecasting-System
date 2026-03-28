#include "DataBaseUserSettings.h"
#include "UI/ui_DataBaseUserSettings.h"

DataBaseUserSettings::DataBaseUserSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DataBaseUserSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

DataBaseUserSettings::~DataBaseUserSettings()
{
    delete ui;
}

void DataBaseUserSettings::on_CloseButton_clicked()
{
    this->close();
}

