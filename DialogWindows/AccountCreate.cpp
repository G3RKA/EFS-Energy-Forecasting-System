#include "AccountCreate.h"
#include "UI/ui_AccountCreate.h"

AccountCreate::AccountCreate(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccountCreate)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->ErrorLabel->setVisible(false);
}

AccountCreate::~AccountCreate()
{
    delete ui;
}

void AccountCreate::on_ConnectButton_clicked()
{

}

void AccountCreate::on_CloseButton_clicked()
{
    this->close();
}
