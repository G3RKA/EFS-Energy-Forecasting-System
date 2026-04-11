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
    explicit DataBaseUserSettings(QWidget *Parent = nullptr);
    ~DataBaseUserSettings();

signals:
    void ConnectRequested(const QString& User, const QString& Password);

public slots:
    void OnCloseUserConnection();
    void OnLoginError();


private slots:
    void on_ConnectButton_clicked();
    void on_CloseButton_clicked();
    void ConnectionDataChanged();

private:
    void SetupDBStatus();

    Ui::DataBaseUserSettings *ui;
};

#endif // DATABASEUSERSETTINGS_H
