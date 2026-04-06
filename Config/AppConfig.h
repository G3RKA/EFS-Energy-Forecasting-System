#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QMap>
#include <qobject.h>

class AppConfig
{
public:
    static const QString DatabaseName;

    static bool IsSuperAdmin(const QString& Login);
    static bool CheckSAPassword(const QString& Login, const QString& Password);
    static QString GetDatabaseSettingsPath();

private:
    AppConfig() = default;

    static const QMap<QString, std::string> SuperAdmins;
};

#endif // APPCONFIG_H
