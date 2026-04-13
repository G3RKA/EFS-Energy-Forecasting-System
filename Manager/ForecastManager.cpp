#include "ForecastManager.h"

#include "AppConfig.h"
#include "Worker/ForecastWorker.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QThread>

QString ForecastManager::FormatRemainingTime(qint64 remainingMs)
{
    const qint64 TotalSec = remainingMs / 1000;
    if (TotalSec < 60) {
        return QString("%1 секунд").arg(TotalSec);
    }
    const qint64 Min = TotalSec / 60;

    return QString("%1 минут").arg(Min);
}

ForecastManager& ForecastManager::instance()
{
    static ForecastManager Inst;
    return Inst;
}

ForecastManager::ForecastManager(QObject* Parent)
    : QObject{Parent}
{
}

ForecastManager::~ForecastManager()
{
    StopWorker();
}

void ForecastManager::Init()
{
    if (m_Initialized) {
        return;
    }
    m_Initialized = true;
    SyncWorkerWithSettings();
}

void ForecastManager::SyncWorkerWithSettings()
{
    if (!m_Initialized) {
        return;
    }
    if (AppConfig::IsForecastEnabled()) {
        StartWorker();
    } else {
        StopWorker();
    }
}

void ForecastManager::StartWorker()
{
    if (m_Thread != nullptr) {
        return;
    }

    m_Thread = new QThread(this);
    m_Worker = new ForecastWorker();
    m_Worker->moveToThread(m_Thread);

    connect(m_Thread, &QThread::started, m_Worker, &ForecastWorker::StartHourBoundaryScheduler);
    connect(m_Worker, &ForecastWorker::FullHourReached, this, &ForecastManager::FullHourReached);
    connect(m_Worker, &ForecastWorker::RemainingTimeToNextFullHour, this,
            &ForecastManager::RemainingTimeToNextFullHour);

    m_Thread->start();
}

void ForecastManager::StopWorker()
{
    if (m_Thread == nullptr) {
        return;
    }

    if (m_Worker != nullptr && m_Thread->isRunning()) {
        QMetaObject::invokeMethod(m_Worker, &ForecastWorker::PrepareShutdown, Qt::BlockingQueuedConnection);
    }

    m_Thread->quit();
    m_Thread->wait();

    if (m_Worker != nullptr) {
        delete m_Worker;
        m_Worker = nullptr;
    }

    delete m_Thread;
    m_Thread = nullptr;
}
