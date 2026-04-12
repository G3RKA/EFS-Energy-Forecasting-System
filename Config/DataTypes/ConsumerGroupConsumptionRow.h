#ifndef CONSUMERGROUPCONSUMPTIONROW_H
#define CONSUMERGROUPCONSUMPTIONROW_H

#include <QDate>
#include <QMetaType>
#include <QString>

struct ConsumerGroupConsumptionRow {
    QString ConsumerGroup;
    QDate Date;
    int Hour = 0;
    int ConsumerCount = 0;
    double Consumption = 0.0;
};

Q_DECLARE_METATYPE(ConsumerGroupConsumptionRow)

#endif // CONSUMERGROUPCONSUMPTIONROW_H
