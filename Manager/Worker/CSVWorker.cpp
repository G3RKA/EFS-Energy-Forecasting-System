#include "CSVWorker.h"

#include <QChar>
#include <QDate>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

CSVWorker::CSVWorker(QObject *parent)
    : QObject{parent}
{
}

void CSVWorker::OnProcessCSVRequested(const QString& filePath)
{
    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        emit CSVProcessingFinished(false, "0");
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit CSVProcessingFinished(false, "0");
        return;
    }

    QTextStream in(&file);

    if (in.atEnd()) {
        emit CSVProcessingFinished(false, "1");
        return;
    }

    QString workLine = in.readLine();
    int lineNumber = 1;

    if (!workLine.isEmpty() and workLine.at(0) == QChar::ByteOrderMark) {
        workLine.remove(0, 1);
    }

    if (workLine.trimmed().isEmpty()) {
        emit CSVProcessingFinished(false, QString::number(lineNumber));
        return;
    }

    const QStringList cells = SplitCSVLine(workLine);
    if (!HeaderMatchesExpected(cells)) {
        emit CSVProcessingFinished(false, QString::number(lineNumber));
        return;
    }

    while (!in.atEnd()) {
        workLine = in.readLine();
        ++lineNumber;

        if (workLine.trimmed().isEmpty()) {
            if (!in.atEnd()) {
                emit CSVProcessingFinished(false, QString::number(lineNumber));
                return;
            }
            continue;
        }

        const QStringList cells = SplitCSVLine(workLine);

        if (cells.size() != 5) {
            emit CSVProcessingFinished(false, QString::number(lineNumber));
            return;
        }

        if (!DataRowIsValid(cells)) {
            emit CSVProcessingFinished(false, QString::number(lineNumber));
            return;
        }
    }

    emit CSVProcessingFinished(true, QString());
}

QStringList CSVWorker::SplitCSVLine(const QString& line)
{
    const QStringList parts = line.split(',', Qt::KeepEmptyParts);
    QStringList result;

    result.reserve(parts.size());
    for (const QString& part : parts) {
        result.append(part.trimmed());
    }
    return result;
}

bool CSVWorker::HeaderMatchesExpected(const QStringList& cells)
{
    const QStringList expected = {
        "Group",
        "Date",
        "Hour",
        "Count",
        "Consumption",
    };
    return cells == expected;
}

bool CSVWorker::DataRowIsValid(const QStringList& cells)
{
    if (cells.size() != 5) {
        return false;
    }

    if (cells.at(0).isEmpty()) {
        return false;
    }

    const QString dateStr = cells.at(1);
    QDate date = QDate::fromString(dateStr, "dd.MM.yyyy");
    if (!date.isValid()) {
        return false;
    }

    bool ok = false;
    const int hour = cells.at(2).toInt(&ok);
    if (!ok or hour < 0 or hour > 23) {
        return false;
    }

    const int count = cells.at(3).toInt(&ok);
    if (!ok or count < 0) {
        return false;
    }

    const double consumption = cells.at(4).toDouble(&ok);
    if (!ok or consumption < 0.0) {
        return false;
    }

    return true;
}
