#ifndef DATAMENU_H
#define DATAMENU_H

#include <QDialog>

namespace Ui {
class DataMenu;
}

class DataMenu : public QDialog
{
    Q_OBJECT

public:
    explicit DataMenu(QWidget *parent = nullptr);
    ~DataMenu();

private slots:
    void on_CloseButton_clicked();

    void on_LoadButton_clicked();
    void OnCSVProcessingFinished(bool success, const QString& message);

private:
    Ui::DataMenu *ui;
};

#endif // DATAMENU_H
