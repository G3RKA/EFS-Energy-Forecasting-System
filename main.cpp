#include "MainMenu/MainWindow.h"

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
    QCoreApplication::setOrganizationName(QStringLiteral("EFS_Project"));
    QCoreApplication::setApplicationName(QStringLiteral("EFS"));

    InitAppFont(a);

    MainWindow w;
    w.show();

    return a.exec();
}
