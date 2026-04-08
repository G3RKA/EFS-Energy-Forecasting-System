#include "AccountSettings.h"
#include "UI/ui_AccountSettings.h"

#include "AccountCreate.h"
#include "AppDataBase.h"

#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QLocale>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidgetItem>
#include <QVariant>

AccountSettings::AccountSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccountSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->AccountTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AccountTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->AccountTable->horizontalHeader()->setMinimumHeight(60);
    ui->AccountTable->setWordWrap(true);
    ui->AccountTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    LoadUsersFromDatabase();
}

AccountSettings::~AccountSettings()
{
    delete ui;
}

void AccountSettings::on_ExitButton_clicked()
{
    this->close();
}


void AccountSettings::on_CreateButton_clicked()
{
    AccountCreate Menu;
    Menu.exec();
}

void AccountSettings::LoadUsersFromDatabase()
{
    ui->AccountTable->setRowCount(0);

    QSqlQuery* query = AppDataBase::GetAllUsers();

    if (not(query))
    {
        return;
    }

    while (query->next()) {
        const int row = ui->AccountTable->rowCount();
        ui->AccountTable->insertRow(row);

        auto makeItem = [](const QString &text) {
            auto *item = new QTableWidgetItem(text);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
            return item;
        };

        ui->AccountTable->setItem(row, 0, makeItem(query->value(0).toString()));
        ui->AccountTable->setItem(row, 1, makeItem(query->value(1).toString()));
        ui->AccountTable->setItem(row, 2, makeItem(FormatRole(query->value(2))));
        ui->AccountTable->setItem(row, 3, makeItem(FormatLastVisit(query->value(3))));
        ui->AccountTable->setItem(row, 4, makeItem(QString()));
        ui->AccountTable->setItem(row, 5, makeItem(QString()));
    }

    delete query;

    ui->AccountTable->resizeRowsToContents();
}

QString AccountSettings::FormatLastVisit(const QVariant &value)
{
    if (value.isNull() || !value.isValid())
        return QString();
    const QDateTime dt = value.toDateTime();
    if (dt.isValid())
       return QLocale().toString(dt, QLocale::ShortFormat);
    return value.toString();
}

QString AccountSettings::FormatRole(const QVariant &value)
{
    QString RoleStr = value.toString();

    if (RoleStr == "analyst")
    {
        return "Аналитик";
    }
    else
    {
        return "Энергетик";
    }
}
