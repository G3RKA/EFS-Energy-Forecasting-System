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
    explicit AuditSettings(QWidget *parent = nullptr);
    ~AuditSettings();

private slots:
    void on_CloseButton_clicked();

private:
    void WriteAuditFlag(const QString& key, bool checked);
    void LoadAuditCheckbox(QCheckBox* box, QSettings& settings, const QString& key);

    Ui::AuditSettings *ui;
};

#endif // AUDITSETTINGS_H
