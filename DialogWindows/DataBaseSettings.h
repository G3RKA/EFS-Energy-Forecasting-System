#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include <QDialog>

namespace Ui {
class DataBaseSettings;
}

class DataBaseSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DataBaseSettings(QWidget *parent = nullptr);
    ~DataBaseSettings();

private slots:
    void on_CloseButton_clicked();

    void on_ConnectButton_clicked();

    void ConnectionDataChanged();

private:
    void SetupDBStatus();

    Ui::DataBaseSettings *ui;
};

#endif // DATABASESETTINGS_H
