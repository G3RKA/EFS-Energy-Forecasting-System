#include "AccountSettings.h"
#include "UI/ui_AccountSettings.h"

#include "AccountCreate.h"
#include "AccountDelete.h"
#include "AccountEdit.h"
#include "AppConfig.h"
#include "DataManager.h"

#include <QDateTime>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLocale>
#include <QPushButton>
#include <QTableWidgetItem>

AccountSettings::AccountSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccountSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    ui->AccountTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AccountTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->AccountTable->horizontalHeader()->setMinimumHeight(60);
    ui->AccountTable->setWordWrap(true);
    ui->AccountTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(&DataManager::instance(), &DataManager::UsersListLoaded, this, &AccountSettings::LoadUsers);

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
    connect(&Menu, &AccountCreate::tableUpdateRequested, this, &AccountSettings::LoadUsersFromDatabase);
    Menu.exec();
}

void AccountSettings::LoadUsersFromDatabase()
{
    DataManager::instance().RequestUsersList();
}

void AccountSettings::LoadUsers(const QVector<AccountUserRow>& users)
{
    ui->AccountTable->setRowCount(0);

    for (const AccountUserRow& user : users) {
        const int row = ui->AccountTable->rowCount();
        ui->AccountTable->insertRow(row);

        auto makeItem = [](const QString &text) {
            auto *item = new QTableWidgetItem(text);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
            return item;
        };
        auto makeButtonContainer = [this](QPushButton* button) {
            auto *container = new QWidget(ui->AccountTable);
            auto *layout = new QHBoxLayout(container);
            layout->setContentsMargins(6, 3, 6, 3);
            layout->setSpacing(0);
            layout->addWidget(button);
            return container;
        };

        const QString& Login = user.login;
        const QString& Name = user.name;

        ui->AccountTable->setItem(row, 0, makeItem(Login));
        ui->AccountTable->setItem(row, 1, makeItem(Name));
        ui->AccountTable->setItem(row, 2, makeItem(FormatRole(QVariant(user.role))));
        ui->AccountTable->setItem(row, 3, makeItem(FormatLastVisit(user.lastVisit)));
        QPushButton *editButton = new QPushButton("Изменить", ui->AccountTable);
        QPushButton *deleteButton = new QPushButton("Удалить", ui->AccountTable);

        SetActionButtonProperties(editButton, Login);
        editButton->setProperty("Name", Name);
        SetActionButtonProperties(deleteButton, Login);

        connect(editButton, &QPushButton::clicked, this, [this, editButton]() {
            OnEditButtonClicked(editButton);
        });

        connect(deleteButton, &QPushButton::clicked, this, [this, deleteButton]() {
            OnDeleteButtonClicked(deleteButton);
        });

        ui->AccountTable->setCellWidget(row, 4, makeButtonContainer(editButton));
        ui->AccountTable->setCellWidget(row, 5, makeButtonContainer(deleteButton));
    }

    ui->AccountTable->resizeRowsToContents();
}

void AccountSettings::EditUserRowByLogin(const QString& login, const QString& name)
{
    for (int row = 0; row < ui->AccountTable->rowCount(); ++row) {
        QTableWidgetItem* loginItem = ui->AccountTable->item(row, 0);
        if (!loginItem || loginItem->text() != login) {
            continue;
        }

        QTableWidgetItem* nameItem = ui->AccountTable->item(row, 1);
        if (nameItem) {
            nameItem->setText(name);
        }

        if (QWidget* container = ui->AccountTable->cellWidget(row, 4)) {
            if (QPushButton* editButton = container->findChild<QPushButton*>()) {
                editButton->setProperty("Name", name);
            }
        }
        return;
    }
}

void AccountSettings::RemoveUserRowByLogin(const QString& login)
{
    for (int row = 0; row < ui->AccountTable->rowCount(); ++row) {
        QTableWidgetItem* item = ui->AccountTable->item(row, 0);
        if (item && item->text() == login) {
            ui->AccountTable->removeRow(row);
            return;
        }
    }
}

void AccountSettings::SetActionButtonProperties(QPushButton* button, const QString& login)
{
    button->setProperty("Login", login);
}

void AccountSettings::OnEditButtonClicked(QPushButton* button)
{
    const QString login = button->property("Login").toString();
    const QString name = button->property("Name").toString();
    qDebug() << "Нажата кнопка Изменить:" << " login =" << login;

    AccountEdit menu(this, login, name);
    connect(&menu, &AccountEdit::UserEdited, this, &AccountSettings::EditUserRowByLogin);
    menu.exec();
}

void AccountSettings::OnDeleteButtonClicked(QPushButton* button)
{
    const QString login = button->property("Login").toString();
    qDebug() << "Нажата кнопка Удалить:" << " login =" << login;

    AccountDelete menu(this, login);
    connect(&menu, &AccountDelete::UserDeleted, this, &AccountSettings::RemoveUserRowByLogin);
    menu.exec();
}

QString AccountSettings::FormatLastVisit(const QVariant &value)
{
    if (value.isNull() || !value.isValid())
        return "Неизвестно";
    const QDateTime dt = value.toDateTime();
    if (dt.isValid())
       return QLocale().toString(dt, QLocale::ShortFormat);
    return value.toString();
}

QString AccountSettings::FormatRole(const QVariant &value)
{
    QString RoleStr = value.toString();

    if (RoleStr == AppConfig::DatabaseAnalystStr)
    {
        return "Аналитик";
    }
    else
    {
        return "Энергетик";
    }
}
