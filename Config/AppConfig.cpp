#include "AppConfig.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "bcrypt.hpp"

const QMap<QString, std::string> AppConfig::SuperAdmins = {
    { "EFS_IT_SA", "$2a$12$wAjxaCLLts6Oe3EJ6A2QSeyxErd4sBVMYMpmP1F0yUTY4z.CnFVIe"} // 12345678Hh
};

const QString AppConfig::DatabaseName = "MainDataBase";

const QString AppConfig::DatabaseAnalystStr = "analyst";
const QString AppConfig::DatabaseEnergyStr = "energy";

const QString AppConfig::DatabaseUsersTable = "user";
const QString AppConfig::DatabaseLogsTable = "logs";

const QString AppConfig::DatabaseGroup = "DataBase";
const QString AppConfig::DatabaseHostKey = "Host";
const QString AppConfig::DatabasePortKey = "Port";
const QString AppConfig::DatabaseNameKey = "Name";
const QString AppConfig::DatabaseLoginKey = "User";
const QString AppConfig::DatabasePasswordKey = "Password";

const QString AppConfig::LoginSecurityGroup = "LoginSecurity";
const QString AppConfig::LoginLockoutAttemptsKey = "failedAttempts";
const QString AppConfig::LoginLockoutUntilMsKey = "lockoutUntilMs";

QSettings AppConfig::LoginLockoutSettings()
{
    return QSettings(QSettings::NativeFormat, QSettings::UserScope,
                     QCoreApplication::organizationName(),
                     QCoreApplication::applicationName());
}

bool AppConfig::IsSuperAdmin(const QString &Login)
{
    return SuperAdmins.contains(Login);
}

bool AppConfig::CheckSAPassword(const QString &Login, const QString &Password)
{
    if (!IsSuperAdmin(Login))
    {
        return false;
    }

    return BCrypt::validatePassword(Password.toStdString(), SuperAdmins.value(Login));
}

QString AppConfig::GetSettingsPath()
{
    QString DataRoot = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(DataRoot);
    return QDir(DataRoot).filePath("Settings.ini");
}

QString AppConfig::ConvertRoleToDB(const QString& role)
{
    const QString normalizedRole = role.trimmed().toLower();

    if (normalizedRole == QString("аналитик").toLower() || normalizedRole == DatabaseAnalystStr) {
        return DatabaseAnalystStr;
    }

    if (normalizedRole == QString("энергетик").toLower() || normalizedRole == DatabaseEnergyStr) {
        return DatabaseEnergyStr;
    }

    return QString();
}
