#ifndef ACCOUNTUSERROW_H
#define ACCOUNTUSERROW_H

#include <QMetaType>
#include <QString>
#include <QVariant>

struct AccountUserRow {
    QString Login;
    QString Name;
    QString Role;
    QVariant LastVisit;
};

Q_DECLARE_METATYPE(AccountUserRow)

#endif // ACCOUNTUSERROW_H
