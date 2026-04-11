#include "UploadError.h"
#include "UI/ui_UploadError.h"

UploadError::UploadError(const QString& LineNumber, QWidget *parent)
    : QDialog(parent)
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

