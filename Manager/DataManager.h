#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QVector>

#include "DataTypes/AccountUserRow.h"
#include "DataTypes/ConsumerGroupRow.h"

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
    void RequestConsumerGroupsList();
    void RequestUpdateConsumerGroupAmount(const QString& GroupName, int Amount);
    void RequestDeleteUser(const QString& Login);
    void RequestUpdateUser(const QString& Login, const QString& Name, const QString& Password);
    void RequestCreateUser(const QString& Login, const QString& Name, const QString& Role, const QString& PasswordHash);
    void RequestLoginVerify(const QString& Login, const QString& Password);
    void RequestProcessCSV(const QString& FilePath);

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
    void OnConsumerGroupsListRequested();
    void OnUpdateConsumerGroupAmountRequested(const QString& GroupName, int Amount);
    void OnDeleteUserRequested(const QString& Login);
    void OnUpdateUserRequested(const QString& Login, const QString& Name, const QString& Password);
    void OnCreateUserRequested(const QString& Login, const QString& Name, const QString& Role, const QString& PasswordHash);
    void OnLoginVerifyRequested(const QString& Login, const QString& Password);
    void OnProcessCSVRequested(const QString& FilePath);

    //  Output
    void OnLostConnection();
    void OnDataBaseStateSended(bool);
    void SettingsAnonymousResult(bool OpenUserDialog, bool ShowErrorLabel, const QString& ErrorLabelText);
    void SettingsUserResult(bool Success, bool UnknownDatabase, bool BadCredentials, const QString& UnknownDbMessage);
    void UsersListLoaded(const QVector<AccountUserRow>& Users);
    void ConsumerGroupsLoaded(const QVector<ConsumerGroupRow>& Groups);
    void UserDeleteFinished(bool Success, const QString& Login);
    void UserUpdateFinished(bool Success, const QString& Login, const QString& Name);
    void UserCreateFinished(bool Success, const QString& Login, bool DuplicateLogin);
    void LoginVerifyFinished(bool Success, const QString& DbRole, const QString& DisplayName, bool InvalidCredentials);
    void CSVProcessingFinished(bool Success, const QString& Message);

private:
    explicit DataManager(QObject *Parent = nullptr);

    QThread* m_Thread = nullptr;
    AppDataBase* m_Worker = nullptr;
    CSVWorker* m_CsvWorker = nullptr;
    bool m_Initialized = false;
};

#endif // DATAMANAGER_H
