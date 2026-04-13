#ifndef ANALYSTMENU_H
#define ANALYSTMENU_H

#include <QString>
#include <QVector>
#include <QWidget>

#include "DataTypes/ConsumerGroupConsumptionRow.h"

namespace Ui {
class AnalystMenu;
}

class AnalystMenu : public QWidget
{
    Q_OBJECT

public:
    explicit AnalystMenu(const QString& AnalystLogin, QWidget *Parent = nullptr);
    ~AnalystMenu();

private slots:
    void on_DataButton_clicked();

    void on_ConsumeButton_clicked();
    void on_TurnOnButton_clicked();
    void on_TurnOffButton_clicked();
    void FillForecastTable(const QVector<ConsumerGroupConsumptionRow>& Rows);
    void OnRemainingTimeToNextFullHour(qint64 remainingMs);
    void OnForecastFullHourReached();

private:
    void RefreshForecastStatusLabel();
    void WriteForecastStatusToIni(bool enabled);

    Ui::AnalystMenu *ui;
    QString m_AnalystLogin;
};

#endif // ANALYSTMENU_H
