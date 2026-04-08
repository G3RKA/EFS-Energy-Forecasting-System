#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <QObject>
#include <QString>
#include <qobject.h>

class QSqlQuery;
class QSqlDatabase;

class AppDataBase : public QObject
{
    Q_OBJECT
public:
    explicit AppDataBase(QObject *parent = nullptr);

    static bool ConnectToDatabase();
    static QSqlQuery* GetAllUsers();
    static bool CreateUser(const QString& login, const QString& name, const QString& role, const QString& password);
    static bool DeleteUserByLogin(const QString& login);
    static bool UpdateUser(const QString& login, const QString& name, const QString& password);

signals:

private:
    static bool GetOpenDatabase(QSqlDatabase& db);
};

#endif // APPDATABASE_H
