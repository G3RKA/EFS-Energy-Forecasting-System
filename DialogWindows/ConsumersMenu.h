#ifndef CONSUMERSMENU_H
#define CONSUMERSMENU_H

#include <QDialog>

namespace Ui {
class ConsumersMenu;
}

class ConsumersMenu : public QDialog
{
    Q_OBJECT

public:
    explicit ConsumersMenu(QWidget *parent = nullptr);
    ~ConsumersMenu();

private slots:
    void on_ExitButton_clicked();

private:
    Ui::ConsumersMenu *ui;
};

#endif // CONSUMERSMENU_H
