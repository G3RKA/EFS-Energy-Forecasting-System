#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QVector>

#include "AccountUserRow.h"

class QThread;
class AppDataBase;

class DataBaseManager : public QObject
{
    Q_OBJECT
public:
    static DataBaseManager& instance();

    void Init();
    void ConnectToDatabase();
    void RequestDBState();
    void RequestTrySettingsAnonymous();
    void RequestTrySettingsWithUser();
    void RequestUsersList();
    void RequestDeleteUser(const QString& login);
    void RequestUpdateUser(const QString& login, const QString& name, const QString& password);
    void RequestCreateUser(const QString& login, const QString& name, const QString& role, const QString& passwordHash);

    ~DataBaseManager() override;

    DataBaseManager(const DataBaseManager&) = delete;
    DataBaseManager& operator=(const DataBaseManager&) = delete;

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

    //  Output
    void OnLostConnection();
    void OnDataBaseStateSended(bool);
    void SettingsAnonymousResult(bool openUserDialog, bool showErrorLabel, const QString& errorLabelText);
    void SettingsUserResult(bool success, bool unknownDatabase, bool badCredentials, const QString& unknownDbMessage);
    void UsersListLoaded(const QVector<AccountUserRow>& users);
    void UserDeleteFinished(bool success, const QString& login);
    void UserUpdateFinished(bool success, const QString& login, const QString& name);
    void UserCreateFinished(bool success, const QString& login, bool duplicateLogin);

private:
    explicit DataBaseManager(QObject *parent = nullptr);

    QThread* m_thread = nullptr;
    AppDataBase* m_worker = nullptr;
    bool m_initialized = false;
};

#endif // DATABASEMANAGER_H
