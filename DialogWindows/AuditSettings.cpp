#include "AuditSettings.h"
#include "UI/ui_AuditSettings.h"

AuditSettings::AuditSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AuditSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

AuditSettings::~AuditSettings()
{
    delete ui;
}

void AuditSettings::on_CloseButton_clicked()
{
    this->close();
}

