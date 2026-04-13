#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QMap>
#include <QString>
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

    static const QString LoginSecurityGroup;
    static const QString LoginLockoutAttemptsKey;
    static const QString LoginLockoutUntilMsKey;
    static const qint64 LoginLockoutDurationMs;

    static const QString ForecastGroup;
    static const QString ForecastStatusKey;

    static const QString AuditGroup;
    static const QString AuditKeyAnalystDisableForecast;
    static const QString AuditKeyAnalystEnableForecast;
    static const QString AuditKeyAnalystUploadData;
    static const QString AuditKeyAnalystChangeData;
    static const QString AuditKeyAnalystLoginLogout;
    static const QString AuditKeyEnergyLoginLogout;

    static bool IsSuperAdmin(const QString& Login);
    static bool CheckSAPassword(const QString& Login, const QString& Password);
    static QString GetSettingsPath();
    static QString ConvertRoleToDB(const QString& Role);

    static bool IsForecastEnabled();
    static QString ForecastStatusCaption();

    static bool IsAuditAnalystLoginLogoutEnabled();
    static bool IsAuditEnergyLoginLogoutEnabled();
    static bool IsAuditAnalystEnableForecastEnabled();
    static bool IsAuditAnalystDisableForecastEnabled();
    static bool IsAuditAnalystUploadDataEnabled();
    static bool IsAuditAnalystChangeDataEnabled();

    static QSettings LoginLockoutSettings();


private:
    AppConfig() = default;

    static const QMap<QString, std::string> SuperAdmins;
};

#endif // APPCONFIG_H
