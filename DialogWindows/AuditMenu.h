#ifndef AUDITMENU_H
#define AUDITMENU_H

#include <QDialog>

namespace Ui {
class AuditMenu;
}

class AuditMenu : public QDialog
{
    Q_OBJECT

public:
    explicit AuditMenu(QWidget *Parent = nullptr);
    ~AuditMenu();

private slots:
    void on_CloseButton_clicked();

private:
    Ui::AuditMenu *ui;
};

#endif // AUDITMENU_H
