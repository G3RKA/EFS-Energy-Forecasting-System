#include "DataBaseSettings.h"
#include "UI/ui_DataBaseSettings.h"

#include "DataBaseUserSettings.h"

#include <QIntValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>

DataBaseSettings::DataBaseSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DataBaseSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    SetupDBStatus();
    SetupInputConstraints();
    
    ui->ErrorLabel->setVisible(false);

    ui->ConnectButton->setDisabled(true);

    connect(ui->HostEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    connect(ui->PortEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
    connect(ui->NameEdit, &QLineEdit::textEdited, this, &DataBaseSettings::ConnectionDataChanged);
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
    TryConnectToMysql();

    //DataBaseUserSettings Menu;
    //Menu.exec();
    ui->ConnectButton->setDisabled(false);
}

QString DataBaseSettings::GetDatabaseSettingsPath()
{
    QString DataRoot = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(DataRoot);
    return QDir(DataRoot).filePath("Settings.ini");
}

void DataBaseSettings::SaveDatabaseConnectionSettings()
{
    QSettings settings(GetDatabaseSettingsPath(), QSettings::IniFormat);
        settings.beginGroup("DataBase");
            settings.setValue("Host", ui->HostEdit->text());
            settings.setValue("Port", ui->PortEdit->text());
            settings.setValue("Name", ui->NameEdit->text());
        settings.endGroup();
    settings.sync();
}

void DataBaseSettings::TryConnectToMysql()
{
    static const QString kConnName = "EFS_DataBase";

    QString driver;
    if (QSqlDatabase::isDriverAvailable("QMYSQL"))
    {
        driver = "QMYSQL";
    }

    if (driver.isEmpty()) {
        qDebug() << "MySQL: драйвер QMYSQL не найден."
                 << "Доступные драйверы:" << QSqlDatabase::drivers();
        return;
    }

    if (QSqlDatabase::contains(kConnName)) {
        {
            QSqlDatabase existing = QSqlDatabase::database(kConnName);
            existing.close();
        }
        QSqlDatabase::removeDatabase(kConnName);
    }

    {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, kConnName);
        db.setHostName(ui->HostEdit->text());
        db.setPort(ui->PortEdit->text().toInt());
        db.setDatabaseName(ui->NameEdit->text());
        if (db.open()) {
            qDebug() << "MySQL: подключение успешно;"
                     << "хост:" << db.hostName() << "порт:" << db.port() << "БД:" << db.databaseName();
            db.close();
        } else {
            qDebug() << "MySQL: подключение не удалось:" << db.lastError().text();
        }
    }

    QSqlDatabase::removeDatabase(kConnName);
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

void DataBaseSettings::SetupDBStatus()
{
    //
}

