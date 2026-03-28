#include "LostConnectionDialog.h"
#include "UI/ui_LostConnectionDialog.h"

LostConnectionDialog::LostConnectionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LostConnectionDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

LostConnectionDialog::~LostConnectionDialog()
{
    delete ui;
}

void LostConnectionDialog::on_OKButton_clicked()
{
    this->close();
}

