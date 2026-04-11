#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <qobject.h>

#include "AccountUserRow.h"

class QSqlDatabase;

class AppDataBase : public QObject
{
    Q_OBJECT
public:
    explicit AppDataBase(QObject *parent = nullptr);

    void SendDataBaseState();

public slots:
    void GetAllUsers();
    void OnConnectRequested();
    void OnTrySettingsAnonymous();
    void OnTrySettingsWithUser();
    void OnDeleteUserRequested(const QString& login);
    void OnUpdateUserRequested(const QString& login, const QString& name, const QString& password);
    void OnCreateUserRequested(const QString& login, const QString& name, const QString& role, const QString& passwordHash);

signals:
    void OnDataBaseStateSended(bool connected);
    void OnLostConnection();
    void SettingsAnonymousResult(bool openUserDialog, bool showErrorLabel, const QString& errorLabelText);
    void SettingsUserResult(bool success, bool unknownDatabase, bool badCredentials, const QString& unknownDbMessage);
    void UsersListLoaded(const QVector<AccountUserRow>& users);
    void UserDeleteFinished(bool success, const QString& login);
    void UserUpdateFinished(bool success, const QString& login, const QString& name);
    void UserCreateFinished(bool success, const QString& login, bool duplicateLogin);

private:
    bool IternalConnectToDatabase();
    bool GetOpenDatabase(QSqlDatabase& db);
};

#endif // APPDATABASE_H
