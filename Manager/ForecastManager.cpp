#include "ForecastManager.h"

ForecastManager& ForecastManager::instance()
{
    static ForecastManager Inst;
    return Inst;
}

ForecastManager::ForecastManager(QObject* Parent)
    : QObject{Parent}
{
}
