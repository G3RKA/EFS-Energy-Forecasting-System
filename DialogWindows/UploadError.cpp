#include "UploadError.h"
#include "UI/ui_UploadError.h"

UploadError::UploadError(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UploadError)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

UploadError::~UploadError()
{
    delete ui;
}

void UploadError::on_OKButton_clicked()
{
    this->close();
}

