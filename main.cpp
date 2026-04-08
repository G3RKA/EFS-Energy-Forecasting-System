#include "LostConnectionDialog.h"
#include "MainMenu/MainWindow.h"
#include "AppDataBase.h"

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

    bool success = AppDataBase::ConnectToDatabase();

    MainWindow w;
    w.show();

    if (not(success))
    {
        LostConnectionDialog menu(&w);

        menu.exec();
    }

    return a.exec();
}
