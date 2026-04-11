#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QVector>

#include "DataTypes/AccountUserRow.h"

class QThread;
class AppDataBase;
class CSVWorker;

class DataManager : public QObject
{
    Q_OBJECT
public:
    static DataManager& instance();

    void Init();
    void ConnectToDatabase();
    void RequestDBState();
    void RequestTrySettingsAnonymous();
    void RequestTrySettingsWithUser();
    void RequestUsersList();
    void RequestDeleteUser(const QString& login);
    void RequestUpdateUser(const QString& login, const QString& name, const QString& password);
    void RequestCreateUser(const QString& login, const QString& name, const QString& role, const QString& passwordHash);
    void RequestLoginVerify(const QString& login, const QString& password);
    void RequestProcessCSV(const QString& filePath);

    ~DataManager() override;

    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

signals:
    // Input
    void OnConnectToDataBase();
    void OnDataBaseRequested();
    void OnTrySettingsAnonymousRequested();
    void OnTrySettingsWithUserRequested();
    void OnUsersListRequested();
    void OnDeleteUserRequested(const QString& login);
    void OnUpdateUserRequested(const QString& login, const QString& name, const QString& password);
    void OnCreateUserRequested(const QString& login, const QString& name, const QString& role, const QString& passwordHash);
    void OnLoginVerifyRequested(const QString& login, const QString& password);
    void OnProcessCSVRequested(const QString& filePath);

    //  Output
    void OnLostConnection();
    void OnDataBaseStateSended(bool);
    void SettingsAnonymousResult(bool openUserDialog, bool showErrorLabel, const QString& errorLabelText);
    void SettingsUserResult(bool success, bool unknownDatabase, bool badCredentials, const QString& unknownDbMessage);
    void UsersListLoaded(const QVector<AccountUserRow>& users);
    void UserDeleteFinished(bool success, const QString& login);
    void UserUpdateFinished(bool success, const QString& login, const QString& name);
    void UserCreateFinished(bool success, const QString& login, bool duplicateLogin);
    void LoginVerifyFinished(bool success, const QString& dbRole, const QString& displayName, bool invalidCredentials);
    void CSVProcessingFinished(bool success, const QString& message);

private:
    explicit DataManager(QObject *parent = nullptr);

    QThread* m_thread = nullptr;
    AppDataBase* m_worker = nullptr;
    CSVWorker* m_CSVWorker = nullptr;
    bool m_initialized = false;
};

#endif // DATAMANAGER_H
