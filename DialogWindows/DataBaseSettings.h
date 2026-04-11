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
    explicit DataBaseSettings(QWidget *Parent = nullptr);
    ~DataBaseSettings();


signals:
    void CloseUserConnection();
    void LoginError();

private slots:
    void on_CloseButton_clicked();
    void on_ConnectButton_clicked();
    void UpdateDBState(bool Connected);
    void ConnectionDataChanged();
    void OnDatabaseUserSubmitted(const QString& User, const QString& Password);
    void OnSettingsAnonymousResult(bool OpenUserDialog, bool ShowErrorLabel, const QString& ErrorLabelText);
    void OnSettingsUserResult(bool Success, bool UnknownDatabase, bool BadCredentials, const QString& UnknownDbMessage);

private:
    void SetupDBStatus();
    void SetupInputConstraints();
    void SaveDatabaseConnectionSettings();

    Ui::DataBaseSettings *ui;
};

#endif // DATABASESETTINGS_H
