#include "SystemLabel.h"
#include "UI/ui_SystemLabel.h"

SystemLabel::SystemLabel(QWidget *Parent)
    : QWidget(Parent)
    , ui(new Ui::SystemLabel)
{
    ui->setupUi(this);
}

SystemLabel::~SystemLabel()
{
    delete ui;
}
