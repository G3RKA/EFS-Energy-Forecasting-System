#ifndef DATABASEUSERSETTINGS_H
#define DATABASEUSERSETTINGS_H

#include <QDialog>

namespace Ui {
class DataBaseUserSettings;
}

class DataBaseUserSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DataBaseUserSettings(QWidget *parent = nullptr);
    ~DataBaseUserSettings();

private slots:
    void on_CloseButton_clicked();

private:
    Ui::DataBaseUserSettings *ui;
};

#endif // DATABASEUSERSETTINGS_H
