#include "UploadError.h"
#include "UI/ui_UploadError.h"

UploadError::UploadError(const QString& LineNumber, QWidget *Parent)
    : QDialog(Parent)
    , ui(new Ui::UploadError)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->NumLabel->setText(LineNumber);
}

UploadError::~UploadError()
{
    delete ui;
}

void UploadError::on_OKButton_clicked()
{
    this->close();
}

