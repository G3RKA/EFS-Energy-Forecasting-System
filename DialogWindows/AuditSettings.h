#ifndef AUDITSETTINGS_H
#define AUDITSETTINGS_H

#include <QDialog>
#include <QCheckBox>
#include <QSettings>
#include <QString>

namespace Ui {
class AuditSettings;
}

class AuditSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AuditSettings(QWidget *Parent = nullptr);
    ~AuditSettings();

private slots:
    void on_CloseButton_clicked();

private:
    void WriteAuditFlag(const QString& Key, bool Checked);
    void LoadAuditCheckbox(QCheckBox* Box, QSettings& Settings, const QString& Key);

    Ui::AuditSettings *ui;
};

#endif // AUDITSETTINGS_H
