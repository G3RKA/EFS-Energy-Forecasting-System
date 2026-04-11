#ifndef CSVWORKER_H
#define CSVWORKER_H

#include <QObject>
#include <QString>
#include <QStringList>

class CSVWorker : public QObject
{
    Q_OBJECT
public:
    explicit CSVWorker(QObject *parent = nullptr);

public slots:
    void OnProcessCSVRequested(const QString& filePath);

signals:
    void CSVProcessingFinished(bool success, const QString& message);

private:
    QStringList SplitCSVLine(const QString& line);
    bool HeaderMatchesExpected(const QStringList& cells);
    bool DataRowIsValid(const QStringList& cells);
};

#endif // CSVWORKER_H
