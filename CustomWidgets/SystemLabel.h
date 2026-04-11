#ifndef SYSTEMLABEL_H
#define SYSTEMLABEL_H

#include <QWidget>

namespace Ui {
class SystemLabel;
}

class SystemLabel : public QWidget
{
    Q_OBJECT

public:
    explicit SystemLabel(QWidget *Parent = nullptr);
    ~SystemLabel();

private:
    Ui::SystemLabel *ui;
};

#endif // SYSTEMLABEL_H
