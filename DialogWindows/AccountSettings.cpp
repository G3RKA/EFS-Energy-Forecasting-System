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

AccountSettings::AccountSettings(QWidget *Parent)
    : QDialog(Parent)
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

void AccountSettings::LoadUsers(const QVector<AccountUserRow>& Users)
{
    ui->AccountTable->setRowCount(0);

    for (const AccountUserRow& User : Users) {
        const int Row = ui->AccountTable->rowCount();
        ui->AccountTable->insertRow(Row);

        auto MakeItem = [](const QString &Text) {
            auto *Item = new QTableWidgetItem(Text);
            Item->setFlags(Item->flags() & ~Qt::ItemIsEditable);
            Item->setTextAlignment(Qt::AlignCenter);
            return Item;
        };
        auto MakeButtonContainer = [this](QPushButton* Button) {
            auto *Container = new QWidget(ui->AccountTable);
            auto *Layout = new QHBoxLayout(Container);
            Layout->setContentsMargins(6, 3, 6, 3);
            Layout->setSpacing(0);
            Layout->addWidget(Button);
            return Container;
        };

        const QString& LoginText = User.Login;
        const QString& NameText = User.Name;

        ui->AccountTable->setItem(Row, 0, MakeItem(LoginText));
        ui->AccountTable->setItem(Row, 1, MakeItem(NameText));
        ui->AccountTable->setItem(Row, 2, MakeItem(FormatRole(QVariant(User.Role))));
        ui->AccountTable->setItem(Row, 3, MakeItem(FormatLastVisit(User.LastVisit)));
        QPushButton *EditButton = new QPushButton("Изменить", ui->AccountTable);
        QPushButton *DeleteButton = new QPushButton("Удалить", ui->AccountTable);

        SetActionButtonProperties(EditButton, LoginText);
        EditButton->setProperty("Name", NameText);
        SetActionButtonProperties(DeleteButton, LoginText);

        connect(EditButton, &QPushButton::clicked, this, [this, EditButton]() {
            OnEditButtonClicked(EditButton);
        });

        connect(DeleteButton, &QPushButton::clicked, this, [this, DeleteButton]() {
            OnDeleteButtonClicked(DeleteButton);
        });

        ui->AccountTable->setCellWidget(Row, 4, MakeButtonContainer(EditButton));
        ui->AccountTable->setCellWidget(Row, 5, MakeButtonContainer(DeleteButton));
    }

    ui->AccountTable->resizeRowsToContents();
}

void AccountSettings::EditUserRowByLogin(const QString& Login, const QString& Name)
{
    for (int Row = 0; Row < ui->AccountTable->rowCount(); ++Row) {
        QTableWidgetItem* LoginItem = ui->AccountTable->item(Row, 0);
        if (!LoginItem || LoginItem->text() != Login) {
            continue;
        }

        QTableWidgetItem* NameItem = ui->AccountTable->item(Row, 1);
        if (NameItem) {
            NameItem->setText(Name);
        }

        if (QWidget* Container = ui->AccountTable->cellWidget(Row, 4)) {
            if (QPushButton* EditButton = Container->findChild<QPushButton*>()) {
                EditButton->setProperty("Name", Name);
            }
        }
        return;
    }
}

void AccountSettings::RemoveUserRowByLogin(const QString& Login)
{
    for (int Row = 0; Row < ui->AccountTable->rowCount(); ++Row) {
        QTableWidgetItem* Item = ui->AccountTable->item(Row, 0);
        if (Item && Item->text() == Login) {
            ui->AccountTable->removeRow(Row);
            return;
        }
    }
}

void AccountSettings::SetActionButtonProperties(QPushButton* Button, const QString& Login)
{
    Button->setProperty("Login", Login);
}

void AccountSettings::OnEditButtonClicked(QPushButton* Button)
{
    const QString Login = Button->property("Login").toString();
    const QString Name = Button->property("Name").toString();
    qDebug() << "Нажата кнопка Изменить:" << " login =" << Login;

    AccountEdit Menu(this, Login, Name);
    connect(&Menu, &AccountEdit::UserEdited, this, &AccountSettings::EditUserRowByLogin);
    Menu.exec();
}

void AccountSettings::OnDeleteButtonClicked(QPushButton* Button)
{
    const QString Login = Button->property("Login").toString();
    qDebug() << "Нажата кнопка Удалить:" << " login =" << Login;

    AccountDelete Menu(this, Login);
    connect(&Menu, &AccountDelete::UserDeleted, this, &AccountSettings::RemoveUserRowByLogin);
    Menu.exec();
}

QString AccountSettings::FormatLastVisit(const QVariant &Value)
{
    if (Value.isNull() || !Value.isValid())
        return "Неизвестно";
    const QDateTime Dt = Value.toDateTime();
    if (Dt.isValid())
       return Dt.toString("dd.MM.yyyy HH:mm");
    return Value.toString();
}

QString AccountSettings::FormatRole(const QVariant &Value)
{
    QString RoleStr = Value.toString();

    if (RoleStr == AppConfig::DatabaseAnalystStr)
    {
        return "Аналитик";
    }
    else
    {
        return "Энергетик";
    }
}
