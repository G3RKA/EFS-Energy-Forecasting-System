#ifndef UPLOADERROR_H
#define UPLOADERROR_H

#include <QDialog>

namespace Ui {
class UploadError;
}

class UploadError : public QDialog
{
    Q_OBJECT

public:
    explicit UploadError(QWidget *parent = nullptr);
    ~UploadError();

private slots:
    void on_OKButton_clicked();

private:
    Ui::UploadError *ui;
};

#endif // UPLOADERROR_H
