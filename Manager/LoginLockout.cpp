#include "LoginLockout.h"

#include "AppConfig.h"

#include <QDateTime>
#include <QSettings>

bool LoginLockout::isLocked(int& RemainingSecondsOut)
{
    QSettings Settings(AppConfig::LoginLockoutSettings());
    Settings.beginGroup(AppConfig::LoginSecurityGroup);
    const qint64 Until = Settings.value(AppConfig::LoginLockoutUntilMsKey, 0).toLongLong();
    const qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if (Until <= 0) {
        Settings.endGroup();
        return false;
    }
    if (Now >= Until) {
        Settings.remove(AppConfig::LoginLockoutUntilMsKey);
        Settings.remove(AppConfig::LoginLockoutAttemptsKey);
        Settings.endGroup();
        return false;
    }

    RemainingSecondsOut = ((Until - Now + 999) / 1000);
    Settings.endGroup();
    return true;
}

void LoginLockout::registerInvalidCredentialsAttempt()
{
    QSettings Settings(AppConfig::LoginLockoutSettings());
    Settings.beginGroup(AppConfig::LoginSecurityGroup);
    const qint64 Now = QDateTime::currentMSecsSinceEpoch();
    const qint64 Until = Settings.value(AppConfig::LoginLockoutUntilMsKey, 0).toLongLong();
    if (Until > Now) {
        Settings.endGroup();
        return;
    }

    const int AttemptCount = Settings.value(AppConfig::LoginLockoutAttemptsKey, 0).toInt() + 1;
    Settings.setValue(AppConfig::LoginLockoutAttemptsKey, AttemptCount);
    if (AttemptCount > 3) {
        Settings.setValue(AppConfig::LoginLockoutUntilMsKey, Now + AppConfig::LoginLockoutDurationMs);
    }
    Settings.endGroup();
}

void LoginLockout::clearOnSuccessfulLogin()
{
    QSettings Settings(AppConfig::LoginLockoutSettings());
    Settings.beginGroup(AppConfig::LoginSecurityGroup);
    Settings.remove(AppConfig::LoginLockoutAttemptsKey);
    Settings.remove(AppConfig::LoginLockoutUntilMsKey);
    Settings.endGroup();
}

QString LoginLockout::messageForRemainingSeconds(int RemainingSeconds)
{
    if (RemainingSeconds <= 0) {
        return "Вход на этом компьютере временно заблокирован.";
    }
    const int Minutes = RemainingSeconds / 60;
    const int Seconds = RemainingSeconds % 60;
    return QString("Вход на этом компьютере заблокирован на 10 минут.\n"
                           "Осталось ждать: %1 мин. %2 сек.")
        .arg(Minutes)
        .arg(Seconds, 2, 10, QLatin1Char('0'));
}
