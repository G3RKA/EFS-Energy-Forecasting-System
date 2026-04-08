#include "AppDataBase.h"

#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <qsqlquery.h>

#include "AppConfig.h"

AppDataBase::AppDataBase(QObject *parent)
    : QObject{parent}
{}

bool AppDataBase::ConnectToDatabase()
{
    const QString DatabaseName = AppConfig::DatabaseName;

    QSettings settings(AppConfig::GetDatabaseSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("DataBase");
    const QString host = settings.value("Host").toString().trimmed();
    const int port = settings.value("Port").toString().trimmed().toInt();
    const QString name = settings.value("Name").toString().trimmed();
    const QString user = settings.value("User").toString().trimmed();
    const QString password = settings.value("Password").toString();
    settings.endGroup();

    const QString driver = "QMYSQL";
    if (!QSqlDatabase::isDriverAvailable(driver)) {
        qDebug() << "Драйвер QMYSQL недоступен, автоподключение пропущено";
        return false;
    }

    if (QSqlDatabase::contains(DatabaseName)) {
        {
            QSqlDatabase existing = QSqlDatabase::database(DatabaseName);
            existing.close();
        }
        QSqlDatabase::removeDatabase(DatabaseName);
    }

    bool success = false;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, DatabaseName);
        db.setHostName(host);
        db.setPort(port);
        db.setDatabaseName(name);
        db.setUserName(user);
        db.setPassword(password);

        if (db.open()) {
            qDebug() << "Подключение к MySQL установлено;"
                     << host << port << name;
            success = true;
        } else {
            qDebug() << "Не удалось подключиться к MySQL:"
                     << db.lastError().text()
                     << "nativeErrorCode:" << db.lastError().nativeErrorCode();
        }
    }

    if (!success)
        QSqlDatabase::removeDatabase(DatabaseName);

    return success;
}

QSqlQuery* AppDataBase::GetAllUsers()
{
    QSqlDatabase db;
    if (!GetOpenDatabase(db)) {
        return nullptr;
    }

    QSqlQuery* query = new QSqlQuery(db);
    query->prepare(QString("SELECT Login, Name, Role, LastVisit FROM %1 ORDER BY Login").arg(AppConfig::DatabaseUsersTable));

    if (!query->exec()) {
        qDebug() << "DataBase: не удалось загрузить пользователей:" << query->lastError().text();
        delete query;
        return nullptr;
    }

    return query;
}

bool AppDataBase::CreateUser(const QString& login, const QString& name, const QString& role, const QString& password)
{
    QSqlDatabase db;
    if (!GetOpenDatabase(db)) {
        return false;
    }

    const QString dbRole = AppConfig::ConvertRoleToDB(role);
    if (dbRole.isEmpty()) {
        qDebug() << "DataBase: некорректная роль пользователя:" << role;
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QString("INSERT INTO %1 (Login, Name, Role, Password) VALUES (:login, :name, :role, :password)")
                      .arg(AppConfig::DatabaseUsersTable));
    query.bindValue(":login", login);
    query.bindValue(":name", name);
    query.bindValue(":role", dbRole);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "DataBase: не удалось создать пользователя:" << query.lastError().text();
        return false;
    }

    return true;
}

bool AppDataBase::DeleteUserByLogin(const QString& login)
{
    QSqlDatabase db;
    if (!GetOpenDatabase(db)) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QString("DELETE FROM %1 WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
    query.bindValue(":login", login);

    if (!query.exec()) {
        qDebug() << "DataBase: не удалось удалить пользователя:" << query.lastError().text();
        return false;
    }

    return true;
}

bool AppDataBase::UpdateUser(const QString& login, const QString& name, const QString& password)
{
    QSqlDatabase db;
    if (!GetOpenDatabase(db)) {
        return false;
    }

    QSqlQuery query(db);
    if (password.isEmpty()) {
        query.prepare(QString("UPDATE %1 SET Name = :name WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
        query.bindValue(":name", name);
        query.bindValue(":login", login);
    } else {
        query.prepare(QString("UPDATE %1 SET Name = :name, Password = :password WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
        query.bindValue(":name", name);
        query.bindValue(":password", password);
        query.bindValue(":login", login);
    }

    if (!query.exec()) {
        qDebug() << "DataBase: не удалось обновить пользователя:" << query.lastError().text();
        return false;
    }

    return true;
}

bool AppDataBase::GetOpenDatabase(QSqlDatabase& db)
{
    if (!QSqlDatabase::contains(AppConfig::DatabaseName)) {
        qDebug() << "DataBase: нет зарегистрированного подключения к БД";
        return false;
    }

    db = QSqlDatabase::database(AppConfig::DatabaseName);
    if (!db.isOpen()) {
        qDebug() << "DataBase: подключение к БД не открыто";
        return false;
    }

    return true;
}