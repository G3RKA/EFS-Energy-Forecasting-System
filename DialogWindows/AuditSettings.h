#ifndef AUDITSETTINGS_H
#define AUDITSETTINGS_H

#include <QDialog>

namespace Ui {
class AuditSettings;
}

class AuditSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AuditSettings(QWidget *parent = nullptr);
    ~AuditSettings();

private slots:
    void on_CloseButton_clicked();

private:
    Ui::AuditSettings *ui;
};

#endif // AUDITSETTINGS_H
