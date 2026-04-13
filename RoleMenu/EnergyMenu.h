#ifndef ENERGYMENU_H
#define ENERGYMENU_H

#include <QWidget>
#include <QVector>

#include "DataTypes/ConsumerGroupConsumptionRow.h"

namespace Ui {
class EnergyMenu;
}

class EnergyMenu : public QWidget
{
    Q_OBJECT

public:
    explicit EnergyMenu(QWidget *Parent = nullptr);
    ~EnergyMenu();

private slots:
    void FillForecastTable(const QVector<ConsumerGroupConsumptionRow>& Rows);

private:
    Ui::EnergyMenu *ui;
};

#endif // ENERGYMENU_H
