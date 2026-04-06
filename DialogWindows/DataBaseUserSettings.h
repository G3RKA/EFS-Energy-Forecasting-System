#ifndef DATABASEUSERSETTINGS_H
#define DATABASEUSERSETTINGS_H

#include <QDialog>

namespace Ui {
class DataBaseUserSettings;
}

class DataBaseUserSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DataBaseUserSettings(QWidget *parent = nullptr);
    ~DataBaseUserSettings();

signals:
    void ConnectRequested(const QString& user, const QString& password);

private slots:
    void on_ConnectButton_clicked();
    void on_CloseButton_clicked();

private:
    void SetupDBStatus();

    Ui::DataBaseUserSettings *ui;
};

#endif // DATABASEUSERSETTINGS_H
