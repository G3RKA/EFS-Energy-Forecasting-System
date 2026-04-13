#ifndef FORECASTMANAGER_H
#define FORECASTMANAGER_H

#include <QObject>

class ForecastManager : public QObject
{
    Q_OBJECT

public:
    static ForecastManager& instance();

    ~ForecastManager() override = default;

    ForecastManager(const ForecastManager&) = delete;
    ForecastManager& operator=(const ForecastManager&) = delete;

private:
    explicit ForecastManager(QObject* Parent = nullptr);
};

#endif // FORECASTMANAGER_H
