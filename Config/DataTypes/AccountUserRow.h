#ifndef ACCOUNTUSERROW_H
#define ACCOUNTUSERROW_H

#include <QMetaType>
#include <QString>
#include <QVariant>

struct AccountUserRow {
    QString login;
    QString name;
    QString role;
    QVariant lastVisit;
};

Q_DECLARE_METATYPE(AccountUserRow)

#endif // ACCOUNTUSERROW_H
