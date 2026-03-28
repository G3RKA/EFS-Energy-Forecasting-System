#include "AccountCreate.h"
#include "UI/ui_AccountCreate.h"

AccountCreate::AccountCreate(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccountCreate)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

AccountCreate::~AccountCreate()
{
    delete ui;
}

void AccountCreate::on_CloseButton_clicked()
{
    this->close();
}

