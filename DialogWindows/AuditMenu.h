#ifndef AUDITMENU_H
#define AUDITMENU_H

#include <QDialog>
#include <QVector>

#include "DataTypes/AuditActionRow.h"

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

    void OnAuditLogForDayLoaded(const QVector<AuditActionRow>& Rows);

private:
    void FillAuditTable(const QVector<AuditActionRow>& Rows);

    Ui::AuditMenu *ui;
};

#endif // AUDITMENU_H
