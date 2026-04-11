#ifndef CONSUMERGROUPROW_H
#define CONSUMERGROUPROW_H

#include <QMetaType>
#include <QString>

struct ConsumerGroupRow {
    QString Name;
    int Amount = 0;
};

Q_DECLARE_METATYPE(ConsumerGroupRow)

#endif // CONSUMERGROUPROW_H
