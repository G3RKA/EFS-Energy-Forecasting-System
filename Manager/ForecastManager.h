#ifndef FORECASTMANAGER_H
#define FORECASTMANAGER_H

#include <QObject>
#include <QString>

class QThread;
class ForecastWorker;

class ForecastManager : public QObject
{
    Q_OBJECT

public:
    static ForecastManager& instance();

    static QString FormatRemainingTime(qint64 remainingMs);

    void Init();

    void SyncWorkerWithSettings();

    ~ForecastManager() override;

    ForecastManager(const ForecastManager&) = delete;
    ForecastManager& operator=(const ForecastManager&) = delete;

signals:
    void FullHourReached();
    void RemainingTimeToNextFullHour(qint64 remainingMs);

private:
    explicit ForecastManager(QObject* Parent = nullptr);

    void StartWorker();
    void StopWorker();

    QThread* m_Thread = nullptr;
    ForecastWorker* m_Worker = nullptr;
    bool m_Initialized = false;
};

#endif // FORECASTMANAGER_H
