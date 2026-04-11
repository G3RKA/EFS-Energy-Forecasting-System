#ifndef CSVWORKER_H
#define CSVWORKER_H

#include <QObject>
#include <QString>

class CSVWorker : public QObject
{
    Q_OBJECT
public:
    explicit CSVWorker(QObject *parent = nullptr);

public slots:
    void OnProcessCSVRequested(const QString& filePath);

signals:
    void CSVProcessingFinished(bool success, const QString& message);
};

#endif // CSVWORKER_H
