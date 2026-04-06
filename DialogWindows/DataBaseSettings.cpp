#include "DataBaseSettings.h"
#include "AppConfig.h"
#include "UI/ui_DataBaseSettings.h"

#include "DataBaseUserSettings.h"

#include <QIntValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>

DataBaseSettings::DataBaseSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DataBaseSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    SetupDBStatus();
    UpdateDBState();
    SetupInputConstraints();
    
    ui->ErrorLabel->setVisible(false);

    ui->ConnectButton->setDisabled(true);

    connect(ui->HostEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    connect(ui->PortEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    connect(ui->NameEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    
    ConnectionDataChanged();
}

DataBaseSettings::~DataBaseSettings()
{
    delete ui;
}

void DataBaseSettings::on_CloseButton_clicked()
{
    this->close();
}

void DataBaseSettings::on_ConnectButton_clicked()
{
    ui->ConnectButton->setDisabled(true);
    SaveDatabaseConnectionSettings();
    if (TryConnectToMysql())
    {
        DataBaseUserSettings menu;
        connect(&menu, &DataBaseUserSettings::ConnectRequested,
                this, &DataBaseSettings::OnDatabaseUserSubmitted);
        menu.exec();
    }
    ui->ConnectButton->setDisabled(false);
}


void DataBaseSettings::ConnectionDataChanged()
{
    auto lineAcceptable = [](QLineEdit *edit) {
        if (edit->text().isEmpty())
            return false;
        int pos = 0;
        QString t = edit->text();
        return edit->validator()->validate(t, pos) == QValidator::Acceptable;
    };

    ui->ConnectButton->setEnabled(lineAcceptable(ui->HostEdit) && lineAcceptable(ui->PortEdit)
                                  && lineAcceptable(ui->NameEdit));
}

void DataBaseSettings::OnDatabaseUserSubmitted(const QString& user, const QString& password)
{
    QSettings settings(AppConfig::GetDatabaseSettingsPath(), QSettings::IniFormat);
        settings.beginGroup(QStringLiteral("DataBase"));
            settings.setValue(QStringLiteral("User"), user);
            settings.setValue(QStringLiteral("Password"), password);
        settings.endGroup();
    settings.sync();

    if (TryConnectToMysqlWithUser(user, password))
    {
        UpdateDBState();
    }
}

void DataBaseSettings::SetupDBStatus()
{
    QSettings settings(AppConfig::GetDatabaseSettingsPath(), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("DataBase"));
        ui->HostEdit->setText(settings.value(QStringLiteral("Host")).toString().trimmed());
        ui->PortEdit->setText(settings.value(QStringLiteral("Port")).toString().trimmed());
        ui->NameEdit->setText(settings.value(QStringLiteral("Name")).toString().trimmed());
    settings.endGroup();
}

void DataBaseSettings::UpdateDBState()
{
    //
}

void DataBaseSettings::SetupInputConstraints()
{
    ui->HostEdit->setMaxLength(255);
    const QRegularExpression hostChars(R"(^[a-zA-Z0-9._]*$)");
    ui->HostEdit->setValidator(new QRegularExpressionValidator(hostChars, ui->HostEdit));

    ui->PortEdit->setMaxLength(5);
    ui->PortEdit->setValidator(new QIntValidator(1, 65535, ui->PortEdit));

    ui->NameEdit->setMaxLength(63);
    const QRegularExpression dbName(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
    ui->NameEdit->setValidator(new QRegularExpressionValidator(dbName, ui->NameEdit));
}

void DataBaseSettings::SaveDatabaseConnectionSettings()
{
    QSettings settings(AppConfig::GetDatabaseSettingsPath(), QSettings::IniFormat);
        settings.beginGroup("DataBase");
            settings.setValue("Host", ui->HostEdit->text());
            settings.setValue("Port", ui->PortEdit->text());
            settings.setValue("Name", ui->NameEdit->text());
        settings.endGroup();
    settings.sync();
}

bool DataBaseSettings::TryConnectToMysql()
{
    const QString kConnName = AppConfig::DatabaseName;
    QString driver = "QMYSQL";

    if (QSqlDatabase::contains(kConnName)) {
        {
            QSqlDatabase existing = QSqlDatabase::database(kConnName);
            existing.close();
        }
        QSqlDatabase::removeDatabase(kConnName);
    }

    bool success = false;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, kConnName);
        db.setHostName(ui->HostEdit->text());
        db.setPort(ui->PortEdit->text().toInt());
        db.setDatabaseName(ui->NameEdit->text());
        db.setUserName(QString());
        db.setPassword(QString());

        if (db.open()) {
            qDebug() << "MySQL: подключение успешно;"
                     << "хост:" << db.hostName() << "порт:" << db.port() << "БД:" << db.databaseName();
            db.close();
            success = true;
        } else {
            const QSqlError err = db.lastError();
            const QString code = err.nativeErrorCode();
            qDebug() << "MySQL:" << err.text() << "nativeErrorCode:" << code;

            if (code == "2002" || code == "2003") {
                // CR_CONN_HOST_ERROR / CR_CONNECTION_ERROR
                qDebug() << "MySQL: не удалось подключиться к серверу по адресу"
                         << QStringLiteral("%1:%2").arg(ui->HostEdit->text(), ui->PortEdit->text());
            } else if (code == "1049") {
                // ER_BAD_DB_ERROR 
                qDebug() << "MySQL: не удалось подключиться к базе данных — БД не найдена или имя указано неверно:"
                         << ui->NameEdit->text();
            } else if (code == "1044" or code =="1045") {
                // ER_DBACCESS_DENIED_ERROR 
                qDebug() << "MySQL: не удалось подключиться к указанной базе данных (нет прав для текущего пользователя).";
                success = true;
            } 
        }
    }

    QSqlDatabase::removeDatabase(kConnName);
    return success;
}

bool DataBaseSettings::TryConnectToMysqlWithUser(const QString& user, const QString& password)
{
    const QString kConnName = AppConfig::DatabaseName;
    const QString driver = "QMYSQL";

    if (QSqlDatabase::contains(kConnName)) {
        {
            QSqlDatabase existing = QSqlDatabase::database(kConnName);
            existing.close();
        }
        QSqlDatabase::removeDatabase(kConnName);
    }

    bool success = false;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, kConnName);
        db.setHostName(ui->HostEdit->text());
        db.setPort(ui->PortEdit->text().toInt());
        db.setDatabaseName(ui->NameEdit->text());
        db.setUserName(user);
        db.setPassword(password);

        if (db.open()) {
            qDebug() << "MySQL: подключение успешно;"
                     << "user:" << user
                     << "host:" << db.hostName() << "port:" << db.port() << "db:" << db.databaseName();
            db.close();
            success = true;
        } else {
            qDebug() << "MySQL: подключение не удалось:"
                     << db.lastError().text()
                     << "nativeErrorCode:" << db.lastError().nativeErrorCode();
        }
    }

    QSqlDatabase::removeDatabase(kConnName);
    return success;
}