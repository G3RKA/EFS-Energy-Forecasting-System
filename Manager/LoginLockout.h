#ifndef LOGINLOCKOUT_H
#define LOGINLOCKOUT_H

#include <QString>

class LoginLockout
{
public:
    static bool isLocked(int& RemainingSecondsOut);
    static void registerInvalidCredentialsAttempt();
    static void clearOnSuccessfulLogin();
    static QString messageForRemainingSeconds(int RemainingSeconds);
};

#endif // LOGINLOCKOUT_H
