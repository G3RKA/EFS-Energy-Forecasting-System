#include "SystemLabel.h"
#include "UI/ui_SystemLabel.h"

SystemLabel::SystemLabel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemLabel)
{
    ui->setupUi(this);
}

SystemLabel::~SystemLabel()
{
    delete ui;
}
