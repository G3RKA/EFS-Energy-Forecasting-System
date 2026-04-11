#ifndef DATAMENU_H
#define DATAMENU_H

#include <QDialog>
#include <QPointer>

#include <QMessageBox>

namespace Ui {
class DataMenu;
}

class DataMenu : public QDialog
{
    Q_OBJECT

public:
    explicit DataMenu(QWidget *Parent = nullptr);
    ~DataMenu();

private slots:
    void on_CloseButton_clicked();

    void on_LoadButton_clicked();
    void OnCSVProcessingFinished(bool Success, const QString& Message);

private:
    Ui::DataMenu *ui;

    void SetupButton(QMessageBox *MessageBox);

    QPointer<QMessageBox> CSVWaitDialog;
};

#endif // DATAMENU_H
