#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QMap>
#include <QSettings>

class AppConfig
{
public:
    static const QString DatabaseName;
    static const QString DatabaseAnalystStr;
    static const QString DatabaseEnergyStr;

    static const QString DatabaseGroup;
    static const QString DatabaseHostKey;
    static const QString DatabasePortKey;
    static const QString DatabaseNameKey;
    static const QString DatabaseLoginKey;
    static const QString DatabasePasswordKey;

    static const QString DatabaseUsersTable;
    static const QString DatabaseLogsTable;
    static const QString DatabaseGroupTable;
    static const QString DatabaseConsumptionTable;
    static const QString DatabaseForecastTable;

    static bool IsSuperAdmin(const QString& Login);
    static bool CheckSAPassword(const QString& Login, const QString& Password);
    static QString GetSettingsPath();
    static QString ConvertRoleToDB(const QString& Role);

    static QSettings LoginLockoutSettings();
    static const QString LoginSecurityGroup;
    static const QString LoginLockoutAttemptsKey;
    static const QString LoginLockoutUntilMsKey;
    static constexpr qint64 LoginLockoutDurationMs = 10LL * 60 * 1000;

private:
    AppConfig() = default;

    static const QMap<QString, std::string> SuperAdmins;
};

#endif // APPCONFIG_H
