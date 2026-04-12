#ifndef CONSUMERGROUPTOTALSROW_H
#define CONSUMERGROUPTOTALSROW_H

#include <QMetaType>
#include <QString>

struct ConsumerGroupTotalsRow {
    QString Group;
    int ConsumerCount = 0;
    double TotalConsumption = 0.0;
};

Q_DECLARE_METATYPE(ConsumerGroupTotalsRow)

#endif // CONSUMERGROUPTOTALSROW_H
