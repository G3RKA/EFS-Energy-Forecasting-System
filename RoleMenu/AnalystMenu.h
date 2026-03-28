#ifndef ANALYSTMENU_H
#define ANALYSTMENU_H

#include <QWidget>

namespace Ui {
class AnalystMenu;
}

class AnalystMenu : public QWidget
{
    Q_OBJECT

public:
    explicit AnalystMenu(QWidget *parent = nullptr);
    ~AnalystMenu();

private slots:
    void on_DataButton_clicked();

    void on_ConsumeButton_clicked();

private:
    Ui::AnalystMenu *ui;
};

#endif // ANALYSTMENU_H
