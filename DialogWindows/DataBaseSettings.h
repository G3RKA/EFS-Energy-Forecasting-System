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
    void ConnectionDataChanged();
    void OnDatabaseUserSubmitted(const QString& user, const QString& password);

private:
    void SetupDBStatus();
    void UpdateDBState();
    void SetupInputConstraints();
    void SaveDatabaseConnectionSettings();
    bool TryConnectToMysql();
    bool TryConnectToMysqlWithUser(const QString& user, const QString& password);

    Ui::DataBaseSettings *ui;
};

#endif // DATABASESETTINGS_H
