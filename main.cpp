#include "MainMenu/MainWindow.h"
#include "DataTypes/AccountUserRow.h"
#include "Manager/DataManager.h"

#include <QFontDatabase>
#include <QDebug>
#include <QApplication>

void InitAppFont(QApplication& app)
{
    int FontID = QFontDatabase::addApplicationFont(":/Resources/Fonts/Merriweather.ttf");

    if (FontID != -1)
    {
        QString family = QFontDatabase::applicationFontFamilies(FontID).at(0);
        app.setFont(QFont(family));
    }
    else
    {
        qDebug() << "Загрузка шрифта завершилась ошибкой";
    }
}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("EFS_Project");
    QCoreApplication::setApplicationName("EFS");

    InitAppFont(a);

    qRegisterMetaType<QVector<AccountUserRow>>("QVector<AccountUserRow>");

    DataManager::instance().Init();
    MainWindow w;

    QObject::connect(&DataManager::instance(), &DataManager::OnLostConnection, &w, &MainWindow::BackToAuthMenu);

    DataManager::instance().ConnectToDatabase();

    w.show();

    return a.exec();
}
