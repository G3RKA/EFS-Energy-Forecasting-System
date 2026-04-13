#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <qobject.h>

#include "DataTypes/AccountUserRow.h"
#include "DataTypes/ConsumerGroupConsumptionRow.h"
#include "DataTypes/ConsumerGroupRow.h"

#include <QDate>

class QSqlDatabase;

class AppDataBase : public QObject
{
    Q_OBJECT
public:
    explicit AppDataBase(QObject *Parent = nullptr);

    void SendDataBaseState();

public slots:
    void GetAllUsers();
    void GetAllConsumerGroups();
    void GetConsumptionDataForGroupAndDate(int GroupId, QDate Date);
    void GetForecastDataForDateAndHour(QDate Date, int Hour);
    void OnConnectRequested();
    void OnTrySettingsAnonymous();
    void OnTrySettingsWithUser();
    void OnDeleteUserRequested(const QString& Login);
    void OnUpdateUserRequested(const QString& Login, const QString& Name, const QString& Password);
    void OnCreateUserRequested(const QString& Login, const QString& Name, const QString& Role, const QString& PasswordHash);
    void OnLoginVerifyRequested(const QString& Login, const QString& Password);
    void OnUpdateConsumerGroupAmountRequested(const QString& GroupName, int Amount);
    void OnUpdateConsumptionEntryRequested(int GroupId, QDate Date, int Hour, int ConsumerAmount, double Consumption);

    // From Worker CSV
    void OnConsumeCSVImport(const QStringList& GroupNames, const QVector<ConsumerGroupConsumptionRow>& Rows);

signals:
    void OnDataBaseStateSended(bool Connected);
    void OnLostConnection();
    void SettingsAnonymousResult(bool OpenUserDialog, bool ShowErrorLabel, const QString& ErrorLabelText);
    void SettingsUserResult(bool Success, bool UnknownDatabase, bool BadCredentials, const QString& UnknownDbMessage);
    void UsersListLoaded(const QVector<AccountUserRow>& Users);
    void ConsumerGroupsLoaded(const QVector<ConsumerGroupRow>& Groups);
    void ConsumptionRowsLoaded(const QVector<ConsumerGroupConsumptionRow>& Rows);
    void ForecastRowsLoaded(const QVector<ConsumerGroupConsumptionRow>& Rows);
    void UserDeleteFinished(bool Success, const QString& Login);
    void UserUpdateFinished(bool Success, const QString& Login, const QString& Name);
    void UserCreateFinished(bool Success, const QString& Login, bool DuplicateLogin);
    void LoginVerifyFinished(bool Success, const QString& DbRole, const QString& DisplayName, bool InvalidCredentials);
    
    void CSVConsumptionImportFinished(bool Success, const QString& Message);

private:
    bool IternalConnectToDatabase();
    bool GetOpenDatabase(QSqlDatabase& Database);
    bool UpdateLastVisitForLogin(const QString& Login);
};

#endif // APPDATABASE_H
