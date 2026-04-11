#include "CSVWorker.h"

CSVWorker::CSVWorker(QObject *parent)
    : QObject{parent}
{
}

void CSVWorker::OnProcessCSVRequested(const QString& filePath)
{
    emit CSVProcessingFinished(true, filePath);
}
