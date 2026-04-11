#ifndef CSVWORKER_H
#define CSVWORKER_H

#include <QObject>
#include <QString>
#include <QStringList>

class CSVWorker : public QObject
{
    Q_OBJECT
public:
    explicit CSVWorker(QObject *Parent = nullptr);

public slots:
    void OnProcessCSVRequested(const QString& FilePath);

signals:
    void CSVProcessingFinished(bool Success, const QString& Message);
    void CSVGroupsToEnsure(const QStringList& GroupNames);

private:
    QStringList SplitCSVLine(const QString& Line);
    bool HeaderMatchesExpected(const QStringList& Cells);
    bool DataRowIsValid(const QStringList& Cells);
};

#endif // CSVWORKER_H
