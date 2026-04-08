#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <QObject>
#include <qsqlquery.h>

class AppDataBase : public QObject
{
    Q_OBJECT
public:
    explicit AppDataBase(QObject *parent = nullptr);

    static bool ConnectToDatabase();
    static QSqlQuery* GetAllUsers();

signals:
};

#endif // APPDATABASE_H
