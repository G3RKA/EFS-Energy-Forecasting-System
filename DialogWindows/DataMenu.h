#ifndef DATAMENU_H
#define DATAMENU_H

#include <QDate>
#include <QDialog>
#include <QPointer>
#include <QString>
#include <QVector>

#include <QMessageBox>

#include "DataTypes/ConsumerGroupConsumptionRow.h"
#include "DataTypes/ConsumerGroupRow.h"

class QTableWidgetItem;

namespace Ui {
class DataMenu;
}

class DataMenu : public QDialog
{
    Q_OBJECT

public:
    explicit DataMenu(const QString& AnalystLogin, QWidget *Parent = nullptr);
    ~DataMenu();

private slots:
    void on_CloseButton_clicked();

    void on_LoadButton_clicked();
    void OnCSVProcessingFinished(bool Success, const QString& Message);
    void FillConsumePicker(const QVector<ConsumerGroupRow>& Groups);
    void FillDataTable(const QVector<ConsumerGroupConsumptionRow>& Rows);
    void OnGroupOrDateFilterChanged();
    void OnDataTableItemChanged(QTableWidgetItem *Item);

private:
    Ui::DataMenu *ui;

    void SetupButton(QMessageBox *MessageBox);
    void RefreshConsumptionTable();

    QPointer<QMessageBox> CSVWaitDialog;
    QString m_AnalystLogin;
};

#endif // DATAMENU_H
