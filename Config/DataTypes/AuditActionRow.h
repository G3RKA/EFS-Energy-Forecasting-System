#ifndef AUDITACTIONROW_H
#define AUDITACTIONROW_H

#include <QDate>
#include <QMetaType>
#include <QString>
#include <QTime>

struct AuditActionRow {
    QString UserRole;
    QString UserName;
    QString UserLogin;
    QString ActionDescription;
    QDate ActionDate;
    QTime ActionTime;
};

Q_DECLARE_METATYPE(AuditActionRow)

#endif // AUDITACTIONROW_H
