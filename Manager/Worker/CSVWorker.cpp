#include "CSVWorker.h"

#include <QChar>
#include <QDate>
#include <QFile>
#include <QIODevice>
#include <QSet>
#include <QTextStream>

CSVWorker::CSVWorker(QObject *Parent)
    : QObject{Parent}
{
}

void CSVWorker::OnProcessCSVRequested(const QString& FilePath)
{
    if (!FilePath.endsWith(".csv", Qt::CaseInsensitive)) {
        emit CSVProcessingFinished(false, "0");
        return;
    }

    QFile File(FilePath);
    if (!File.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit CSVProcessingFinished(false, "0");
        return;
    }

    QTextStream In(&File);

    if (In.atEnd()) {
        emit CSVProcessingFinished(false, "0");
        return;
    }

    QString WorkLine = In.readLine();
    int LineNumber = 1;

    if (!WorkLine.isEmpty() and WorkLine.at(0) == QChar::ByteOrderMark) {
        WorkLine.remove(0, 1);
    }

    if (WorkLine.trimmed().isEmpty()) {
        emit CSVProcessingFinished(false, QString::number(LineNumber));
        return;
    }

    const QStringList Cells = SplitCSVLine(WorkLine);
    if (!HeaderMatchesExpected(Cells)) {
        emit CSVProcessingFinished(false, QString::number(LineNumber));
        return;
    }

    QSet<QString> UniqueGroups;

    while (!In.atEnd()) {
        WorkLine = In.readLine();
        ++LineNumber;

        if (WorkLine.trimmed().isEmpty()) {
            if (!In.atEnd()) {
                emit CSVProcessingFinished(false, QString::number(LineNumber));
                return;
            }
            continue;
        }

        const QStringList RowCells = SplitCSVLine(WorkLine);

        if (RowCells.size() != 5) {
            emit CSVProcessingFinished(false, QString::number(LineNumber));
            return;
        }

        if (!DataRowIsValid(RowCells)) {
            emit CSVProcessingFinished(false, QString::number(LineNumber));
            return;
        }

        UniqueGroups.insert(RowCells.at(0));
    }

    QStringList GroupNames;
    GroupNames.reserve(UniqueGroups.size());
    for (const QString& G : UniqueGroups) {
        GroupNames.append(G);
    }

    emit CSVGroupsToEnsure(GroupNames);

    emit CSVProcessingFinished(true, QString());
}

QStringList CSVWorker::SplitCSVLine(const QString& Line)
{
    const QStringList Parts = Line.split(',', Qt::KeepEmptyParts);
    QStringList Result;

    Result.reserve(Parts.size());
    for (const QString& Part : Parts) {
        Result.append(Part.trimmed());
    }
    return Result;
}

bool CSVWorker::HeaderMatchesExpected(const QStringList& Cells)
{
    const QStringList Expected = {
        "Group",
        "Date",
        "Hour",
        "Count",
        "Consumption",
    };
    return Cells == Expected;
}

bool CSVWorker::DataRowIsValid(const QStringList& Cells)
{
    if (Cells.size() != 5) {
        return false;
    }

    if (Cells.at(0).isEmpty()) {
        return false;
    }

    const QString DateStr = Cells.at(1);
    QDate Date = QDate::fromString(DateStr, "dd.MM.yyyy");
    if (!Date.isValid()) {
        return false;
    }

    bool Ok = false;
    const int Hour = Cells.at(2).toInt(&Ok);
    if (!Ok or Hour < 0 or Hour > 23) {
        return false;
    }

    const int Count = Cells.at(3).toInt(&Ok);
    if (!Ok or Count < 0) {
        return false;
    }

    const double Consumption = Cells.at(4).toDouble(&Ok);
    if (!Ok or Consumption < 0.0) {
        return false;
    }

    return true;
}
