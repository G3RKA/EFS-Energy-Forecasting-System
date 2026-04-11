#ifndef CONSUMERSMENU_H
#define CONSUMERSMENU_H

#include <QDialog>
#include <QVector>

#include "DataTypes/ConsumerGroupRow.h"

class QTableWidgetItem;

namespace Ui {
class ConsumersMenu;
}

class ConsumersMenu : public QDialog
{
    Q_OBJECT

public:
    explicit ConsumersMenu(QWidget *Parent = nullptr);
    ~ConsumersMenu();

private slots:
    void on_ExitButton_clicked();
    void LoadConsumerGroups(const QVector<ConsumerGroupRow>& Groups);
    void OnConsumeTableItemChanged(QTableWidgetItem *Item);

private:
    void ConsumerAmountChanged(const QString& GroupName, int Amount);

    Ui::ConsumersMenu *ui;
};

#endif // CONSUMERSMENU_H
