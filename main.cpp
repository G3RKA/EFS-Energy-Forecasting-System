#include "MainMenu/MainWindow.h"
#include "DataTypes/AccountUserRow.h"
#include "DataTypes/AuditActionRow.h"
#include "DataTypes/ConsumerGroupConsumptionRow.h"
#include "DataTypes/ConsumerGroupRow.h"
#include "DataTypes/ConsumerGroupTotalsRow.h"
#include "Manager/DataManager.h"
#include "Manager/ForecastManager.h"

#include <QDate>
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
    qRegisterMetaType<QVector<AuditActionRow>>("QVector<AuditActionRow>");
    qRegisterMetaType<QVector<ConsumerGroupConsumptionRow>>("QVector<ConsumerGroupConsumptionRow>");
    qRegisterMetaType<QVector<ConsumerGroupRow>>("QVector<ConsumerGroupRow>");
    qRegisterMetaType<QVector<ConsumerGroupTotalsRow>>("QVector<ConsumerGroupTotalsRow>");
    qRegisterMetaType<QDate>("QDate");

    DataManager::instance().Init();
    ForecastManager::instance().Init();
    MainWindow W;

    QObject::connect(&DataManager::instance(), &DataManager::OnLostConnection, &W, &MainWindow::BackToAuthMenu);

    DataManager::instance().ConnectToDatabase();

    W.show();

    return App.exec(); 
}
