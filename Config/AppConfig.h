#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QMap>

class AppConfig
{
public:
    static bool IsSuperAdmin(const QString& Login);
    static bool CheckSAPassword(const QString& Login, const QString& Password);

private:
    AppConfig() = default;

    static const QMap<QString, std::string> SuperAdmins;
};

#endif // APPCONFIG_H
