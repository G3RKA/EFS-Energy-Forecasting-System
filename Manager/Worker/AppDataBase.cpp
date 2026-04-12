#include "AppDataBase.h"

#include <QDateTime>
#include <QMetaObject>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "AppConfig.h"
#include "bcrypt.hpp"

AppDataBase::AppDataBase(QObject *Parent)
    : QObject{Parent}
{

}

void AppDataBase::SendDataBaseState()
{
    const QString ConnName = AppConfig::DatabaseName;
    bool Connected = false;
    if (QSqlDatabase::contains(ConnName)) {
        QSqlDatabase Database = QSqlDatabase::database(ConnName);
        Connected = Database.isValid() && Database.isOpen();
    }
    emit OnDataBaseStateSended(Connected);
}


void AppDataBase::GetAllUsers()
{
    QVector<AccountUserRow> Rows;
    QSqlDatabase Database;
    if (!GetOpenDatabase(Database)) {
        emit UsersListLoaded(Rows);
        return;
    }

    QSqlQuery Query(Database);
    Query.prepare(QStringLiteral("SELECT Login, Name, Role, LastVisit FROM %1 ORDER BY Login")
                      .arg(AppConfig::DatabaseUsersTable));

    if (!Query.exec()) {
        qDebug() << "DataBase: не удалось загрузить пользователей:" << Query.lastError().text();
        emit UsersListLoaded(Rows);
        return;
    }

    while (Query.next()) {
        AccountUserRow Row;
        Row.Login = Query.value(0).toString();
        Row.Name = Query.value(1).toString();
        Row.Role = Query.value(2).toString();
        Row.LastVisit = Query.value(3);
        Rows.append(std::move(Row));
    }
    emit UsersListLoaded(Rows);
    return;
}

void AppDataBase::GetAllConsumerGroups()
{
    QVector<ConsumerGroupRow> Rows;
    QSqlDatabase Database;
    if (!GetOpenDatabase(Database)) {
        emit ConsumerGroupsLoaded(Rows);
        return;
    }

    QSqlQuery Query(Database);
    Query.prepare(QStringLiteral("SELECT Name, Amount FROM %1 ORDER BY Name")
                      .arg(AppConfig::DatabaseGroupTable));

    if (!Query.exec()) {
        qDebug() << "DataBase: не удалось загрузить группы:" << Query.lastError().text();
        return;
    }

    while (Query.next()) {
        ConsumerGroupRow Row;
        Row.Name = Query.value(0).toString();
        Row.Amount = Query.value(1).toInt();
        Rows.append(std::move(Row));
    }
    emit ConsumerGroupsLoaded(Rows);
}

void AppDataBase::OnUpdateConsumerGroupAmountRequested(const QString& GroupName, int Amount)
{
    QSqlDatabase Database;
    if (!GetOpenDatabase(Database)) {
        return;
    }

    if (Amount < 0) {
        return;
    }

    QSqlQuery Query(Database);
    Query.prepare(QStringLiteral("UPDATE %1 SET Amount = :amount WHERE Name = :name")
                      .arg(AppConfig::DatabaseGroupTable));
    Query.bindValue(":amount", Amount);
    Query.bindValue(":name", GroupName);

    if (!Query.exec()) {
        qDebug() << "DataBase: не удалось обновить количество в группе:" << Query.lastError().text();
        return;
    }
}

void AppDataBase::OnDeleteUserRequested(const QString& Login)
{
    bool Ok = false;
    QSqlDatabase Database;
    if (GetOpenDatabase(Database)) {
        QSqlQuery Query(Database);
        Query.prepare(QStringLiteral("DELETE FROM %1 WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
        Query.bindValue(QStringLiteral(":login"), Login);
        if (Query.exec()) {
            Ok = true;
        } else {
            qDebug() << "DataBase: не удалось удалить пользователя:" << Query.lastError().text();
        }
    }
    emit UserDeleteFinished(Ok, Login);
}

void AppDataBase::OnUpdateUserRequested(const QString& Login, const QString& Name, const QString& Password)
{
    bool Ok = false;
    QSqlDatabase Database;
    if (GetOpenDatabase(Database)) {
        QSqlQuery Query(Database);
        if (Password.isEmpty()) {
            Query.prepare(QStringLiteral("UPDATE %1 SET Name = :name WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
            Query.bindValue(QStringLiteral(":name"), Name);
            Query.bindValue(QStringLiteral(":login"), Login);
        } else {
            Query.prepare(QStringLiteral("UPDATE %1 SET Name = :name, Password = :password WHERE Login = :login")
                              .arg(AppConfig::DatabaseUsersTable));
            Query.bindValue(QStringLiteral(":name"), Name);
            Query.bindValue(QStringLiteral(":password"), Password);
            Query.bindValue(QStringLiteral(":login"), Login);
        }

        if (Query.exec()) {
            Ok = true;
        } else {
            qDebug() << "DataBase: не удалось обновить пользователя:" << Query.lastError().text();
        }
    }
    emit UserUpdateFinished(Ok, Login, Name);
}

void AppDataBase::OnLoginVerifyRequested(const QString& Login, const QString& Password)
{
    QSqlDatabase Database;
    if (!GetOpenDatabase(Database)) {
        return;
    }

    QSqlQuery Query(Database);
    Query.prepare(QStringLiteral("SELECT Role, Password, Name FROM %1 WHERE Login = :login")
                      .arg(AppConfig::DatabaseUsersTable));
    Query.bindValue(QStringLiteral(":login"), Login);
    if (!Query.exec()) {
        qDebug() << "DataBase: не удалось проверить вход:" << Query.lastError().text();
        emit LoginVerifyFinished(false, QString(), QString(), false);
        return;
    }

    if (!Query.next()) {
        emit LoginVerifyFinished(false, QString(), QString(), true);
        return;
    }

    const QString DbRole = Query.value(0).toString().trimmed().toLower();
    const QString PasswordHash = Query.value(1).toString();
    const QString DisplayName = Query.value(2).toString();

    if (PasswordHash.isEmpty()
        or !BCrypt::validatePassword(Password.toStdString(), PasswordHash.toStdString())) {
        emit LoginVerifyFinished(false, QString(), QString(), true);
        return;
    }

    UpdateLastVisitForLogin(Login);

    emit LoginVerifyFinished(true, DbRole, DisplayName, false);
}

void AppDataBase::OnEnsureGroupsFromCSV(const QStringList& GroupNames)
{
    QSqlDatabase Database;
    if (!GetOpenDatabase(Database)) {
        return;
    }

    for (const QString& GroupName : GroupNames) {
        if (GroupName.isEmpty()) {
            continue;
        }

        QSqlQuery ExistsQuery(Database);
        ExistsQuery.prepare(QString("SELECT 1 FROM %1 WHERE `Name` = :name LIMIT 1")
                                .arg(AppConfig::DatabaseGroupTable));
        ExistsQuery.bindValue(":name", GroupName);
        if (!ExistsQuery.exec()) {
            qDebug() << "DataBase: не удалось проверить группу:" << ExistsQuery.lastError().text();
            return;
        }
        if (ExistsQuery.next()) {
            continue;
        }

        QSqlQuery InsertQuery(Database);
        InsertQuery.prepare(QString("INSERT INTO %1 (`Name`) VALUES (:name)")
                                 .arg(AppConfig::DatabaseGroupTable));
        InsertQuery.bindValue(":name", GroupName);
        if (!InsertQuery.exec()) {
            qDebug() << "DataBase: не удалось добавить группу:" << InsertQuery.lastError().text();
            return;
        }
    }

    emit CSVGroupsPersistFinished(true, QString());
}

void AppDataBase::OnCreateUserRequested(const QString& Login, const QString& Name, const QString& Role,
                                        const QString& PasswordHash)
{
    QSqlDatabase Database;
    if (!GetOpenDatabase(Database)) {
        return;
    }

    QSqlQuery TakenQuery(Database);
    TakenQuery.prepare(QString("SELECT 1 FROM %1 WHERE Login = :login").arg(AppConfig::DatabaseUsersTable));
    TakenQuery.bindValue(":login", Login);
    if (!TakenQuery.exec()) {
        qDebug() << "DataBase: не удалось проверить уникальность логина:" << TakenQuery.lastError().text();
        return;
    }
    if (TakenQuery.next()) {
        emit UserCreateFinished(false, Login, true);
        return;
    }

    const QString DbRole = AppConfig::ConvertRoleToDB(Role);
    if (DbRole.isEmpty()) {
        qDebug() << "DataBase: некорректная роль пользователя:" << Role;
        return;
    }

    QSqlQuery Insert(Database);
    Insert.prepare(QString("INSERT INTO %1 (Login, Name, Role, Password) VALUES (:login, :name, :role, :password)")
                       .arg(AppConfig::DatabaseUsersTable));
    Insert.bindValue(":login", Login);
    Insert.bindValue(":name", Name);
    Insert.bindValue(":role", DbRole);
    Insert.bindValue(":password", PasswordHash);

    if (!Insert.exec()) {
        qDebug() << "DataBase: не удалось создать пользователя:" << Insert.lastError().text();
        return;
    }

    emit UserCreateFinished(true, Login, false);
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
    const QString MainConnectionName = AppConfig::DatabaseName;

    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup("DataBase");
    const QString Host = Settings.value("Host").toString().trimmed();
    const int Port = Settings.value("Port").toString().trimmed().toInt();
    const QString CatalogName = Settings.value("Name").toString().trimmed();
    const QString User = Settings.value("User").toString().trimmed();
    const QString DbPassword = Settings.value("Password").toString();
    Settings.endGroup();

    const QString Driver = "QMYSQL";
    if (!QSqlDatabase::isDriverAvailable(Driver)) {
        qDebug() << "Драйвер QMYSQL недоступен, автоподключение пропущено";
        return false;
    }

    if (QSqlDatabase::contains(MainConnectionName)) {
        {
            QSqlDatabase Existing = QSqlDatabase::database(MainConnectionName);
            Existing.close();
        }
        QSqlDatabase::removeDatabase(MainConnectionName);
    }

    bool Success = false;
    {
        QSqlDatabase Database = QSqlDatabase::addDatabase(Driver, MainConnectionName);
        Database.setHostName(Host);
        Database.setPort(Port);
        Database.setDatabaseName(CatalogName);
        Database.setUserName(User);
        Database.setPassword(DbPassword);

        if (Database.open()) {
            qDebug() << "Подключение к MySQL установлено;"
                     << Host << Port << CatalogName;
            Success = true;
        } else {
            qDebug() << "Не удалось подключиться к MySQL:"
                     << Database.lastError().text()
                     << "nativeErrorCode:" << Database.lastError().nativeErrorCode();
        }
    }

    if (!Success)
        QSqlDatabase::removeDatabase(MainConnectionName);

    return Success;
}

bool AppDataBase::GetOpenDatabase(QSqlDatabase& Database)
{
    if (!QSqlDatabase::contains(AppConfig::DatabaseName)) {
        qDebug() << "DataBase: нет зарегистрированного подключения к БД";
        emit OnLostConnection();
        return false;
    }

    Database = QSqlDatabase::database(AppConfig::DatabaseName);
    if (!Database.isOpen()) {
        qDebug() << "DataBase: подключение к БД не открыто";
        emit OnLostConnection();
        return false;
    }

    QSqlQuery Ping(Database);
    if (!Ping.exec("SELECT 1")) {
        qDebug() << "DataBase: пинг сервера не удался:" << Ping.lastError().text();
        if (!IternalConnectToDatabase()) {
            qDebug() << "DataBase: не удалось переподключиться к БД";
            emit OnLostConnection();
            return false;
        }
    }
    return true;
}

bool AppDataBase::UpdateLastVisitForLogin(const QString& Login)
{
    QSqlDatabase Database;
    if (!GetOpenDatabase(Database)) {
        return false;
    }

    QSqlQuery Query(Database);
    Query.prepare(QString("UPDATE %1 SET LastVisit = :lastVisit WHERE Login = :login")
                      .arg(AppConfig::DatabaseUsersTable));
    const QDateTime LocalNow = QDateTime::currentDateTime();
    Query.bindValue(":lastVisit", LocalNow.toString("yyyy-MM-dd HH:mm:ss"));
    Query.bindValue(":login", Login);
    if (!Query.exec()) {
        qDebug() << "DataBase: не удалось обновить LastVisit:" << Query.lastError().text();
        return false;
    }
    return true;
}

void AppDataBase::OnTrySettingsAnonymous()
{
    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup(AppConfig::DatabaseGroup);
        const QString Host = Settings.value(AppConfig::DatabaseHostKey).toString().trimmed();
        const QString PortText = Settings.value(AppConfig::DatabasePortKey).toString().trimmed();
        const QString DbCatalogName = Settings.value(AppConfig::DatabaseNameKey).toString().trimmed();
    Settings.endGroup();

    const QString ConnRegistryName = AppConfig::DatabaseName;
    const QString Driver = "QMYSQL";

    bool OpenUserDialog = false;
    bool ShowError = false;
    QString ErrorText;

    if (QSqlDatabase::contains(ConnRegistryName)) {
        {
            QSqlDatabase Existing = QSqlDatabase::database(ConnRegistryName);
            Existing.close();
        }
        QSqlDatabase::removeDatabase(ConnRegistryName);
    }

    {
        QSqlDatabase Database = QSqlDatabase::addDatabase(Driver, ConnRegistryName);
        Database.setHostName(Host);
        Database.setPort(PortText.toInt());
        Database.setDatabaseName(DbCatalogName);
        Database.setUserName(QString());
        Database.setPassword(QString());

        if (Database.open()) {
            qDebug() << "MySQL: подключение успешно;"
                     << "хост:" << Database.hostName() << "порт:" << Database.port() << "БД:" << Database.databaseName();
            Database.close();
            OpenUserDialog = true;
        } else {
            const QSqlError Err = Database.lastError();
            const QString Code = Err.nativeErrorCode();
            qDebug() << "MySQL:" << Err.text() << "nativeErrorCode:" << Code;

            if (Code == "2002" or Code == "2003" or Code == "2005") {
                qDebug() << "MySQL: не удалось подключиться к серверу по адресу"
                         << QString("%1:%2").arg(Host, PortText);
                ShowError = true;
                ErrorText = QString("Не удалось присоединиться к серверу по %1")
                                .arg(QString("%1:%2").arg(Host, PortText));
            } else if (Code == "1044" or Code == "1045") {
                qDebug() << "MySQL: Попытка присоединения к базе без пользователя.";
                OpenUserDialog = true;
            }
        }
    }

    QSqlDatabase::removeDatabase(ConnRegistryName);
    emit SettingsAnonymousResult(OpenUserDialog, ShowError, ErrorText);
}

void AppDataBase::OnTrySettingsWithUser()
{
    QSettings Settings(AppConfig::GetSettingsPath(), QSettings::IniFormat);
    Settings.beginGroup(AppConfig::DatabaseGroup);
        const QString Host = Settings.value(AppConfig::DatabaseHostKey).toString().trimmed();
        const QString PortText = Settings.value(AppConfig::DatabasePortKey).toString().trimmed();
        const QString CatalogName = Settings.value(AppConfig::DatabaseNameKey).toString().trimmed();
        const QString User = Settings.value(AppConfig::DatabaseLoginKey).toString().trimmed();
        const QString DbPassword = Settings.value(AppConfig::DatabasePasswordKey).toString().trimmed();
    Settings.endGroup();

    const QString ConnRegistryName = AppConfig::DatabaseName;
    const QString Driver = "QMYSQL";

    if (QSqlDatabase::contains(ConnRegistryName)) {
        {
            QSqlDatabase Existing = QSqlDatabase::database(ConnRegistryName);
            Existing.close();
        }
        QSqlDatabase::removeDatabase(ConnRegistryName);
    }

    bool Success = false;
    bool UnknownDb = false;
    bool BadAuth = false;
    QString UnknownMsg;

    {
        QSqlDatabase Database = QSqlDatabase::addDatabase(Driver, ConnRegistryName);
        Database.setHostName(Host);
        Database.setPort(PortText.toInt());
        Database.setDatabaseName(CatalogName);
        Database.setUserName(User);
        Database.setPassword(DbPassword);

        if (Database.open()) {
            qDebug() << "MySQL: подключение успешно;"
                     << "user:" << User
                     << "host:" << Database.hostName() << "port:" << Database.port() << "db:" << Database.databaseName();
            Success = true;
        } else {
            const QSqlError Err = Database.lastError();
            const QString Code = Err.nativeErrorCode();

            if (Code == "1049") {
                qDebug() << "MySQL: не удалось подключиться к базе данных — БД не найдена:" << CatalogName;
                UnknownDb = true;
                UnknownMsg = QString("Не удалось присоединиться к базе данных %1").arg(CatalogName);
            }

            if (Code == "1044" or Code == "1045") {
                qDebug() << "MySQL: Неверный логин или пароль для доступа.";
                BadAuth = true;
            }

            qDebug() << "MySQL: подключение не удалось:"
                     << Database.lastError().text()
                     << "nativeErrorCode:" << Database.lastError().nativeErrorCode();

            Database.close();
        }
    }

    if (!Success) {
        QSqlDatabase::removeDatabase(ConnRegistryName);
    }

    emit SettingsUserResult(Success, UnknownDb, BadAuth, UnknownMsg);
}
