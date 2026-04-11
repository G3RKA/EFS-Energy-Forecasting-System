#ifndef LOGINLOCKOUT_H
#define LOGINLOCKOUT_H

#include <QString>

class LoginLockout
{
public:
    static bool isLocked(int& remainingSecondsOut);
    static void registerInvalidCredentialsAttempt();
    static void clearOnSuccessfulLogin();
    static QString messageForRemainingSeconds(int remainingSeconds);
};

#endif // LOGINLOCKOUT_H
