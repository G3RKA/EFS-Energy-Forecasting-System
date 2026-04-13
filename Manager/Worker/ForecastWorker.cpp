#include "ForecastWorker.h"

#include <climits>

#include <QDateTime>
#include <QDebug>
#include <QTimer>

ForecastWorker::ForecastWorker(QObject* Parent)
    : QObject{Parent}
{
}

QDateTime ForecastWorker::NextFullHourAfter(const QDateTime& Now)
{
    return QDateTime(Now.date(), QTime(Now.time().hour(), 0, 0)).addSecs(3600);
}

void ForecastWorker::StartHourBoundaryScheduler()
{
    if (!m_TickTimer) {
        m_TickTimer = new QTimer(this);
        m_TickTimer->setInterval(1000);
        m_TickTimer->setTimerType(Qt::CoarseTimer);
        connect(m_TickTimer, &QTimer::timeout, this, &ForecastWorker::OnSecondTick);
    }
    if (!m_HourTimer) {
        m_HourTimer = new QTimer(this);
        m_HourTimer->setSingleShot(true);
        connect(m_HourTimer, &QTimer::timeout, this, &ForecastWorker::OnFullHourTimeout);
    }

    ScheduleNextFullHour();
    OnSecondTick();
    m_TickTimer->start();
}

void ForecastWorker::PrepareShutdown()
{
    if (m_TickTimer) {
        m_TickTimer->stop();
    }
    if (m_HourTimer) {
        m_HourTimer->stop();
    }
}

void ForecastWorker::OnSecondTick()
{
    const QDateTime Now = QDateTime::currentDateTime();
    const QDateTime Next = NextFullHourAfter(Now);
    qint64 Ms = Now.msecsTo(Next);
    if (Ms < 0) {
        Ms = 0;
    }
    emit RemainingTimeToNextFullHour(Ms);
}

void ForecastWorker::ScheduleNextFullHour()
{
    if (!m_HourTimer) {
        return;
    }
    const QDateTime Now = QDateTime::currentDateTime();
    const QDateTime Next = NextFullHourAfter(Now);
    qint64 Ms = Now.msecsTo(Next);
    if (Ms < 1) {
        Ms = 1;
    }
    m_HourTimer->start(Ms);
}

void ForecastWorker::OnFullHourTimeout()
{
    const QDateTime Now = QDateTime::currentDateTime();
    const QTime T = Now.time();

    if (T.minute() != 0) {
        ScheduleNextFullHour();
        return;
    }

    const int H = T.hour();

    emit FullHourReached();
    qDebug() << "Достигнут час" << H;
    ScheduleNextFullHour();
}
