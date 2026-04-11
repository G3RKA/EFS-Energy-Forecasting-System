#include "MainMenu/MainWindow.h"
#include "DataTypes/AccountUserRow.h"
#include "DataTypes/ConsumerGroupRow.h"
#include "Manager/DataManager.h"

#include <QFontDatabase>
#include <QDebug>
#include <QApplication>
#include <qdatetime.h>

void InitAppFont(QApplication& App)
{
    int FontId = QFontDatabase::addApplicationFont(":/Resources/Fonts/Merriweather.ttf");

    if (FontId != -1)
    {
        QString Family = QFontDatabase::applicationFontFamilies(FontId).at(0);
        App.setFont(QFont(Family));
    }
    else
    {
        qDebug() << "Загрузка шрифта завершилась ошибкой";
    }
}

int main(int argc, char *argv[])
{

    QApplication App(argc, argv);
    QCoreApplication::setOrganizationName("EFS_Project");
    QCoreApplication::setApplicationName("EFS");

    InitAppFont(App);

    qRegisterMetaType<QVector<AccountUserRow>>("QVector<AccountUserRow>");
    qRegisterMetaType<QVector<ConsumerGroupRow>>("QVector<ConsumerGroupRow>");

    DataManager::instance().Init();
    MainWindow W;

    QObject::connect(&DataManager::instance(), &DataManager::OnLostConnection, &W, &MainWindow::BackToAuthMenu);

    DataManager::instance().ConnectToDatabase();

    W.show();

    qDebug() << QDateTime::currentDateTime();

    return App.exec(); 
}
