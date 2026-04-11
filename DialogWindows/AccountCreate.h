#ifndef ACCOUNTCREATE_H
#define ACCOUNTCREATE_H

#include <QDialog>
#include <QString>

namespace Ui {
class AccountCreate;
}

class AccountCreate : public QDialog
{
    Q_OBJECT

public:
    explicit AccountCreate(QWidget *Parent = nullptr);
    ~AccountCreate();

signals:
    void tableUpdateRequested();

private slots:
    void on_CloseButton_clicked();
    void on_ConnectButton_clicked();
    void OnUserCreateFinished(bool Success, const QString& Login, bool DuplicateLogin);

private:
    bool IsLoginValid(const QString& Login);
    bool IsPasswordValid(const QString& Password);
    bool IsNameValid(const QString& Name);

    QString FailMessage;
    QString PendingCreateLogin;

    Ui::AccountCreate *ui;
};

#endif // ACCOUNTCREATE_H
