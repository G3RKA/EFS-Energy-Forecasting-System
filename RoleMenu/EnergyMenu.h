#ifndef ENERGYMENU_H
#define ENERGYMENU_H

#include <QWidget>

namespace Ui {
class EnergyMenu;
}

class EnergyMenu : public QWidget
{
    Q_OBJECT

public:
    explicit EnergyMenu(QWidget *parent = nullptr);
    ~EnergyMenu();

private:
    Ui::EnergyMenu *ui;
};

#endif // ENERGYMENU_H
