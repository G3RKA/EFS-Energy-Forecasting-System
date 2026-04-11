#include "AppDataBase.h"

#include <QMetaObject>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "AppConfig.h"
#include "bcrypt.hpp"

AppDataBase::AppDataBase(QObject *parent)
    : QObject{parent}
{

}

void AppDataBase::SendDataBaseState()
{
    const QString connName = AppConfig::DatabaseName;
    bool connected = false;
    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase db = QSqlDatabase::database(connName);
        connected = db.isValid() && db.isOpen();
    }
    emit OnDataBaseStateSended(connected);
}


void AppDataBase::GetAllUsers()
{
    QVector<AccountUserRow> rows;
    QSqlDatabase db;
    if (!GetOpenDatabase(db)) {
        emit UsersListLoaded(rows);
        return;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT Login, Name, Role, LastVisit FROM %1 ORDER BY Login")
                      .arg(AppConfig::DatabaseUsersTable));

    if (!query.exec()) {
        qDebug() << "DataBase: не удалось загрузить пользователей:" << query.lastError().text();
        emit UsersListLoaded(rows);
        return;
    }

    while (query.next()) {
        AccountUserRow row;
        row.login = query.value(0).toString();
        row.name = query.value(1).toString();
        row.role = query.value(2).toString();
        row.lastVisit = query.value(3);
        rows.append(std::move(row));
    }
    emit UsersListLoaded(rows);
    return;
}

void AppDataBase::OnDeleteUserRequested(const QString& login)
{
    bool ok = false;
    QSqlDatabase db;
    if (GetOpenDatabase(db)) {
        QSqlQuery query(db);
        query.prepare(QStringLiteral("DELETE FROM %1 WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
        query.bindValue(QStringLiteral(":login"), login);
        if (query.exec()) {
            ok = true;
        } else {
            qDebug() << "DataBase: не удалось удалить пользователя:" << query.lastError().text();
        }
    }
    emit UserDeleteFinished(ok, login);
}

void AppDataBase::OnUpdateUserRequested(const QString& login, const QString& name, const QString& password)
{
    bool ok = false;
    QSqlDatabase db;
    if (GetOpenDatabase(db)) {
        QSqlQuery query(db);
        if (password.isEmpty()) {
            query.prepare(QStringLiteral("UPDATE %1 SET Name = :name WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
            query.bindValue(QStringLiteral(":name"), name);
            query.bindValue(QStringLiteral(":login"), login);
        } else {
            query.prepare(QStringLiteral("UPDATE %1 SET Name = :name, Password = :password WHERE Login = :login")
                              .arg(AppConfig::DatabaseUsersTable));
            query.bindValue(QStringLiteral(":name"), name);
            query.bindValue(QStringLiteral(":password"), password);
            query.bindValue(QStringLiteral(":login"), login);
        }

        if (query.exec()) {
            ok = true;
        } else {
            qDebug() << "DataBase: не удалось обновить пользователя:" << query.lastError().text();
        }
    }
    emit UserUpdateFinished(ok, login, name);
}

void AppDataBase::OnLoginVerifyRequested(const QString& login, const QString& password)
{
    QSqlDatabase db;
    if (!GetOpenDatabase(db)) {
        return;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT Role, Password, Name FROM %1 WHERE Login = :login")
                      .arg(AppConfig::DatabaseUsersTable));
    query.bindValue(QStringLiteral(":login"), login);
    if (!query.exec()) {
        qDebug() << "DataBase: не удалось проверить вход:" << query.lastError().text();
        emit LoginVerifyFinished(false, QString(), QString(), false);
        return;
    }

    if (!query.next()) {
        emit LoginVerifyFinished(false, QString(), QString(), true);
        return;
    }

    const QString dbRole = query.value(0).toString().trimmed().toLower();
    const QString passwordHash = query.value(1).toString();
    const QString displayName = query.value(2).toString();

    if (passwordHash.isEmpty()
        or !BCrypt::validatePassword(password.toStdString(), passwordHash.toStdString())) {
        emit LoginVerifyFinished(false, QString(), QString(), true);
        return;
    }

    emit LoginVerifyFinished(true, dbRole, displayName, false);
}

void AppDataBase::OnCreateUserRequested(const QString& login, const QString& name, const QString& role,
                                        const QString& passwordHash)
{
    QSqlDatabase db;
    if (!GetOpenDatabase(db)) {
        return;
    }

    QSqlQuery takenQuery(db);
    takenQuery.prepare(QString("SELECT 1 FROM %1 WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
    takenQuery.bindValue(":login", login);
    if (!takenQuery.exec()) {
        qDebug() << "DataBase: не удалось проверить уникальность логина:" << takenQuery.lastError().text();
        return;
    }
    if (takenQuery.next()) {
        emit UserCreateFinished(false, login, true);
        return;
    }

    const QString dbRole = AppConfig::ConvertRoleToDB(role);
    if (dbRole.isEmpty()) {
        qDebug() << "DataBase: некорректная роль пользователя:" << role;
        return;
    }

    QSqlQuery insert(db);
    insert.prepare(QString("INSERT INTO %1 (Login, Name, Role, Password) VALUES (:login, :name, :role, :password)")
                       .arg(AppConfig::DatabaseUsersTable));
    insert.bindValue(":login", login);
    insert.bindValue(":name", name);
    insert.bindValue(":role", dbRole);
    insert.bindValue(":password", passwordHash);

    if (!insert.exec()) {
        qDebug() << "DataBase: не удалось создать пользователя:" << insert.lastError().text();
        return;
    }

    emit UserCreateFinished(true, login, false);
}

void AppDataBase::OnConnectRequested()
{
    if (!IternalConnectToDatabase()) {
        qDebug() << "DataBase: не удалось переподключиться к БД";
        emit OnLostConnection();
        return;
    }    
}

bool AppDataBase::IternalConnectToDatabase()
{
    const QString DatabaseName = AppConfig::DatabaseName;

    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
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

bool AppDataBase::GetOpenDatabase(QSqlDatabase& db)
{
    if (!QSqlDatabase::contains(AppConfig::DatabaseName)) {
        qDebug() << "DataBase: нет зарегистрированного подключения к БД";
        emit OnLostConnection();
        return false;
    }

    db = QSqlDatabase::database(AppConfig::DatabaseName);
    if (!db.isOpen()) {
        qDebug() << "DataBase: подключение к БД не открыто";
        emit OnLostConnection();
        return false;
    }

    QSqlQuery ping(db);
    if (!ping.exec("SELECT 1")) {
        qDebug() << "DataBase: пинг сервера не удался:" << ping.lastError().text();
        if (!IternalConnectToDatabase()) {
            qDebug() << "DataBase: не удалось переподключиться к БД";
            emit OnLostConnection();
            return false;
        }
    }
    return true;
}

void AppDataBase::OnTrySettingsAnonymous()
{
    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("DataBase");
        const QString host = settings.value("Host").toString().trimmed();
        const QString portText = settings.value("Port").toString().trimmed();
        const QString dbName = settings.value("Name").toString().trimmed();
    settings.endGroup();

    const QString kConnName = AppConfig::DatabaseName;
    const QString driver = "QMYSQL";

    bool openUserDialog = false;
    bool showError = false;
    QString errorText;

    if (QSqlDatabase::contains(kConnName)) {
        {
            QSqlDatabase existing = QSqlDatabase::database(kConnName);
            existing.close();
        }
        QSqlDatabase::removeDatabase(kConnName);
    }

    {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, kConnName);
        db.setHostName(host);
        db.setPort(portText.toInt());
        db.setDatabaseName(dbName);
        db.setUserName(QString());
        db.setPassword(QString());

        if (db.open()) {
            qDebug() << "MySQL: подключение успешно;"
                     << "хост:" << db.hostName() << "порт:" << db.port() << "БД:" << db.databaseName();
            db.close();
            openUserDialog = true;
        } else {
            const QSqlError err = db.lastError();
            const QString code = err.nativeErrorCode();
            qDebug() << "MySQL:" << err.text() << "nativeErrorCode:" << code;

            if (code == "2002" or code == "2003" or code == "2005") {
                qDebug() << "MySQL: не удалось подключиться к серверу по адресу"
                         << QString("%1:%2").arg(host, portText);
                showError = true;
                errorText = QString("Не удалось присоединиться к серверу по %1")
                                .arg(QString("%1:%2").arg(host, portText));
            } else if (code == "1044" or code == "1045") {
                qDebug() << "MySQL: Попытка присоединения к базе без пользователя.";
                openUserDialog = true;
            }
        }
    }

    QSqlDatabase::removeDatabase(kConnName);
    emit SettingsAnonymousResult(openUserDialog, showError, errorText);
}

void AppDataBase::OnTrySettingsWithUser()
{
    QSettings settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    settings.beginGroup("DataBase");
        const QString host = settings.value("Host").toString().trimmed();
        const QString portText = settings.value("Port").toString().trimmed();
        const QString name = settings.value("Name").toString().trimmed();
        const QString user = settings.value("User").toString().trimmed();
        const QString password = settings.value("Password").toString().trimmed();
    settings.endGroup();

    const QString kConnName = AppConfig::DatabaseName;
    const QString driver = QStringLiteral("QMYSQL");

    if (QSqlDatabase::contains(kConnName)) {
        {
            QSqlDatabase existing = QSqlDatabase::database(kConnName);
            existing.close();
        }
        QSqlDatabase::removeDatabase(kConnName);
    }

    bool success = false;
    bool unknownDb = false;
    bool badAuth = false;
    QString unknownMsg;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, kConnName);
        db.setHostName(host);
        db.setPort(portText.toInt());
        db.setDatabaseName(name);
        db.setUserName(user);
        db.setPassword(password);

        if (db.open()) {
            qDebug() << "MySQL: подключение успешно;"
                     << "user:" << user
                     << "host:" << db.hostName() << "port:" << db.port() << "db:" << db.databaseName();
            success = true;
        } else {
            const QSqlError err = db.lastError();
            const QString code = err.nativeErrorCode();

            if (code == "1049") {
                qDebug() << "MySQL: не удалось подключиться к базе данных — БД не найдена:" << name;
                unknownDb = true;
                unknownMsg = QString("Не удалось присоединиться к базе данных %1").arg(name);
            }

            if (code == "1044" or code == "1045") {
                qDebug() << "MySQL: Неверный логин или пароль для доступа.";
                badAuth = true;
            }

            qDebug() << "MySQL: подключение не удалось:"
                     << db.lastError().text()
                     << "nativeErrorCode:" << db.lastError().nativeErrorCode();

            db.close();
        }
    }

    if (!success) {
        QSqlDatabase::removeDatabase(kConnName);
    }

    emit SettingsUserResult(success, unknownDb, badAuth, unknownMsg);
}