#ifndef LOGINLOCKOUT_H
#define LOGINLOCKOUT_H

#include <QString>

class LoginLockout
{
public:
    static bool IsLocked(int& RemainingSecondsOut);
    static void RegisterInvalidCredentialsAttempt();
    static void ClearOnSuccessfulLogin();
    static QString MessageForRemainingSeconds(int RemainingSeconds);
};

#endif // LOGINLOCKOUT_H
