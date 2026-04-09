#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QMap>
#include <qobject.h>

class AppConfig
{
public:
    static const QString DatabaseName;
    static const QString DatabaseAnalystStr;
    static const QString DatabaseEnergyStr;
    static const QString DatabaseUsersTable;
    static const QString DatabaseLogsTable;

    static bool IsSuperAdmin(const QString& Login);
    static bool CheckSAPassword(const QString& Login, const QString& Password);
    static QString GetSettingsPath();
    static QString ConvertRoleToDB(const QString& role);

private:
    AppConfig() = default;

    static const QMap<QString, std::string> SuperAdmins;
};

#endif // APPCONFIG_H
