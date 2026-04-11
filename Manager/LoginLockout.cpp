#include "LoginLockout.h"

#include "AppConfig.h"

#include <QDateTime>

bool LoginLockout::isLocked(int& remainingSecondsOut)
{
    QSettings s(AppConfig::LoginLockoutSettings());
    s.beginGroup(AppConfig::LoginSecurityGroup);
    const qint64 until = s.value(AppConfig::LoginLockoutUntilMsKey, 0).toLongLong();
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (until <= 0) {
        s.endGroup();
        return false;
    }
    if (now >= until) {
        s.remove(AppConfig::LoginLockoutUntilMsKey);
        s.remove(AppConfig::LoginLockoutAttemptsKey);
        s.endGroup();
        return false;
    }

    remainingSecondsOut = ((until - now + 999) / 1000);
    s.endGroup();
    return true;
}

void LoginLockout::registerInvalidCredentialsAttempt()
{
    QSettings s(AppConfig::LoginLockoutSettings());
    s.beginGroup(AppConfig::LoginSecurityGroup);
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const qint64 until = s.value(AppConfig::LoginLockoutUntilMsKey, 0).toLongLong();
    if (until > now) {
        s.endGroup();
        return;
    }

    const int n = s.value(AppConfig::LoginLockoutAttemptsKey, 0).toInt() + 1;
    s.setValue(AppConfig::LoginLockoutAttemptsKey, n);
    if (n > 3) {
        s.setValue(AppConfig::LoginLockoutUntilMsKey, now + AppConfig::LoginLockoutDurationMs);
    }
    s.endGroup();
}

void LoginLockout::clearOnSuccessfulLogin()
{
    QSettings s(AppConfig::LoginLockoutSettings());
    s.beginGroup(AppConfig::LoginSecurityGroup);
    s.remove(AppConfig::LoginLockoutAttemptsKey);
    s.remove(AppConfig::LoginLockoutUntilMsKey);
    s.endGroup();
}

QString LoginLockout::messageForRemainingSeconds(int remainingSeconds)
{
    if (remainingSeconds <= 0) {
        return "Вход на этом компьютере временно заблокирован.";
    }
    const int m = remainingSeconds / 60;
    const int sec = remainingSeconds % 60;
    return QString("Вход на этом компьютере заблокирован на 10 минут.\n"
                           "Осталось ждать: %1 мин. %2 сек.")
        .arg(m)
        .arg(sec, 2, 10, QLatin1Char('0'));
}
