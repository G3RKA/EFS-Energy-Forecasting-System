#ifndef ADMINMENU_H
#define ADMINMENU_H

#include <QVector>
#include <QWidget>

#include "DataTypes/AuditActionRow.h"

namespace Ui {
class AdminMenu;
}

class AdminMenu : public QWidget
{
    Q_OBJECT

public:
    explicit AdminMenu(QWidget *Parent = nullptr);
    ~AdminMenu();

private slots:
    void on_AuditSetButton_clicked();
    void on_AuditButton_clicked();
    void on_DataBaseButton_clicked();
    void on_AccountsButton_clicked();
    void on_MoreButton_clicked();
    void OnAuditLogAllLoaded(const QVector<AuditActionRow>& Rows);

private:
    Ui::AdminMenu *ui;
};

#endif // ADMINMENU_H
