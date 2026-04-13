#ifndef FORECASTWORKER_H
#define FORECASTWORKER_H

#include <QDateTime>
#include <QObject>

class QTimer;

class ForecastWorker : public QObject
{
    Q_OBJECT

public:
    explicit ForecastWorker(QObject* Parent = nullptr);

public slots:
    void StartHourBoundaryScheduler();
    void PrepareShutdown();

signals:
    void FullHourReached();
    void RemainingTimeToNextFullHour(qint64 remainingMs);

private slots:
    void OnFullHourTimeout();
    void OnSecondTick();

private:
    static QDateTime NextFullHourAfter(const QDateTime& Now);

    void ScheduleNextFullHour();

    QTimer* m_TickTimer = nullptr;
    QTimer* m_HourTimer = nullptr;
};

#endif // FORECASTWORKER_H
