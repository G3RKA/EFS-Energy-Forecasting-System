#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void BackToAuthMenu();

private slots:
    void on_LogInButton_clicked();
    void on_RecoveryButton_clicked();
    void on_LogOutButton_clicked();
    void OnLoginVerifyFinished(bool success, const QString& dbRole, const QString& displayName,
                               bool invalidCredentials);

private:
    void SetUserName(const QString&);
    void EnableWrongPassword();
    void ShowCredentialError();
    void ClearEdits();
    void DisableLabels();
    void ResetRoleMenu();

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
