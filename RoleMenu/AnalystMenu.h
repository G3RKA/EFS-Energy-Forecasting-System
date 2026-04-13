#ifndef ANALYSTMENU_H
#define ANALYSTMENU_H

#include <QWidget>
#include <QVector>

#include "DataTypes/ConsumerGroupConsumptionRow.h"

namespace Ui {
class AnalystMenu;
}

class AnalystMenu : public QWidget
{
    Q_OBJECT

public:
    explicit AnalystMenu(QWidget *Parent = nullptr);
    ~AnalystMenu();

private slots:
    void on_DataButton_clicked();

    void on_ConsumeButton_clicked();
    void FillForecastTable(const QVector<ConsumerGroupConsumptionRow>& Rows);

private:
    Ui::AnalystMenu *ui;
};

#endif // ANALYSTMENU_H
