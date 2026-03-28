#include "DataBaseSettings.h"
#include "UI/ui_DataBaseSettings.h"

#include "DataBaseUserSettings.h"

DataBaseSettings::DataBaseSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DataBaseSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    SetupDBStatus();
    ui->ErrorLabel->setVisible(false);

    ui->ConnectButton->setDisabled(true);

    connect(ui->HostEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    connect(ui->PortEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    connect(ui->NameEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
}

DataBaseSettings::~DataBaseSettings()
{
    delete ui;
}

void DataBaseSettings::on_CloseButton_clicked()
{
    this->close();
}

void DataBaseSettings::on_ConnectButton_clicked()
{
    //DataBaseUserSettings Menu;
    //Menu.exec();
}

void DataBaseSettings::ConnectionDataChanged()
{
    ui->ConnectButton->setEnabled(not(ui->HostEdit->text().isEmpty() or ui->PortEdit->text().isEmpty() or ui->NameEdit->text().isEmpty()));
}

void DataBaseSettings::SetupDBStatus()
{
    //
}

