#ifndef LOSTCONNECTIONDIALOG_H
#define LOSTCONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class LostConnectionDialog;
}

class LostConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LostConnectionDialog(QWidget *parent = nullptr);
    ~LostConnectionDialog();

private slots:
    void on_OKButton_clicked();

private:
    Ui::LostConnectionDialog *ui;
};

#endif // LOSTCONNECTIONDIALOG_H
