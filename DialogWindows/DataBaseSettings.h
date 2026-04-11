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


signals:
    void CloseUserConnection();
    void LoginError();

private slots:
    void on_CloseButton_clicked();
    void on_ConnectButton_clicked();
    void UpdateDBState(bool connected);
    void ConnectionDataChanged();
    void OnDatabaseUserSubmitted(const QString& user, const QString& password);
    void OnSettingsAnonymousResult(bool openUserDialog, bool showErrorLabel, const QString& errorLabelText);
    void OnSettingsUserResult(bool success, bool unknownDatabase, bool badCredentials, const QString& unknownDbMessage);

private:
    void SetupDBStatus();
    void SetupInputConstraints();
    void SaveDatabaseConnectionSettings();

    Ui::DataBaseSettings *ui;
};

#endif // DATABASESETTINGS_H
