#ifndef CSVWORKER_H
#define CSVWORKER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

#include "DataTypes/ConsumerGroupConsumptionRow.h"

class CSVWorker : public QObject
{
    Q_OBJECT
public:
    explicit CSVWorker(QObject *Parent = nullptr);

public slots:
    void OnProcessCSVRequested(const QString& FilePath);

signals:
    void CSVProcessingFinished(bool Success, const QString& Message);
    void CSVParsedForImport(const QStringList& GroupNames, const QVector<ConsumerGroupConsumptionRow>& Rows);

private:
    QStringList SplitCSVLine(const QString& Line);
    bool HeaderMatchesExpected(const QStringList& Cells);
    bool DataRowIsValid(const QStringList& Cells);
};

#endif // CSVWORKER_H
