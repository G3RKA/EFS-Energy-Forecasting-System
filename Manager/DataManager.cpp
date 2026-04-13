#include "DataManager.h"

#include "Worker/AppDataBase.h"
#include "Worker/CSVWorker.h"

#include <QMetaObject>
#include <QThread>

DataManager& DataManager::instance()
{
    static DataManager Inst;
    return Inst;
}

DataManager::DataManager(QObject *Parent)
    : QObject{Parent}
{}

DataManager::~DataManager()
{
    if (m_Thread) {
        m_Thread->quit();
        m_Thread->wait();
    }
    delete m_CsvWorker;
    m_CsvWorker = nullptr;
    delete m_Worker;
    m_Worker = nullptr;
    m_Thread = nullptr;
}

void DataManager::Init()
{
    if (m_Initialized) {
        return;
    }
    m_Initialized = true;

    m_Thread = new QThread(this);
    m_Worker = new AppDataBase();
    m_Worker->moveToThread(m_Thread);

    m_CsvWorker = new CSVWorker();
    m_CsvWorker->moveToThread(m_Thread);

    // Worker -> Manager
    connect(m_Worker, &AppDataBase::OnDataBaseStateSended, this, &DataManager::OnDataBaseStateSended);
    connect(m_Worker, &AppDataBase::OnLostConnection, this, &DataManager::OnLostConnection);
    connect(m_Worker, &AppDataBase::SettingsAnonymousResult, this, &DataManager::SettingsAnonymousResult);
    connect(m_Worker, &AppDataBase::SettingsUserResult, this, &DataManager::SettingsUserResult);
    connect(m_Worker, &AppDataBase::UsersListLoaded, this, &DataManager::UsersListLoaded);
    connect(m_Worker, &AppDataBase::AuditLogAllLoaded, this, &DataManager::AuditLogAllLoaded);
    connect(m_Worker, &AppDataBase::AuditLogForDayLoaded, this, &DataManager::AuditLogForDayLoaded);
    connect(m_Worker, &AppDataBase::ConsumerGroupsLoaded, this, &DataManager::ConsumerGroupsLoaded);
    connect(m_Worker, &AppDataBase::ConsumptionRowsLoaded, this, &DataManager::ConsumptionRowsLoaded);
    connect(m_Worker, &AppDataBase::ForecastRowsLoaded, this, &DataManager::ForecastRowsLoaded);
    connect(m_Worker, &AppDataBase::UserDeleteFinished, this, &DataManager::UserDeleteFinished);
    connect(m_Worker, &AppDataBase::UserUpdateFinished, this, &DataManager::UserUpdateFinished);
    connect(m_Worker, &AppDataBase::UserCreateFinished, this, &DataManager::UserCreateFinished);
    connect(m_Worker, &AppDataBase::LoginVerifyFinished, this, &DataManager::LoginVerifyFinished);
    connect(m_CsvWorker, &CSVWorker::CSVProcessingFinished, this, &DataManager::CSVProcessingFinished);
    connect(m_Worker, &AppDataBase::CSVConsumptionImportFinished, this, &DataManager::CSVProcessingFinished);

    // Worker -> Worker
    connect(m_CsvWorker, &CSVWorker::CSVParsedForImport, m_Worker, &AppDataBase::OnConsumeCSVImport);

    // Manager -> Worker
    connect(this, &DataManager::OnConnectToDataBase, m_Worker, &AppDataBase::OnConnectRequested);
    connect(this, &DataManager::OnDataBaseRequested, m_Worker, &AppDataBase::SendDataBaseState);
    connect(this, &DataManager::OnTrySettingsAnonymousRequested, m_Worker, &AppDataBase::OnTrySettingsAnonymous);
    connect(this, &DataManager::OnTrySettingsWithUserRequested, m_Worker, &AppDataBase::OnTrySettingsWithUser);
    connect(this, &DataManager::OnUsersListRequested, m_Worker, &AppDataBase::GetAllUsers);
    connect(this, &DataManager::OnAuditLogRequested, m_Worker, &AppDataBase::GetAuditLogRows);
    connect(this, &DataManager::OnAppendAuditLogRequested, m_Worker, &AppDataBase::AppendAuditLogRow);
    connect(this, &DataManager::OnConsumerGroupsListRequested, m_Worker, &AppDataBase::GetAllConsumerGroups);
    connect(this, &DataManager::OnConsumptionDataForGroupAndDateRequested, m_Worker,
            &AppDataBase::GetConsumptionDataForGroupAndDate);
    connect(this, &DataManager::OnForecastDataForDateAndHourRequested, m_Worker, &AppDataBase::GetForecastDataForDateAndHour);
    connect(this, &DataManager::OnGenerateForecastForDateAndHourRequested, m_Worker,
            &AppDataBase::GenerateForecastForDateAndHour);
    connect(this, &DataManager::OnUpdateConsumerGroupAmountRequested, m_Worker, &AppDataBase::OnUpdateConsumerGroupAmountRequested);
    connect(this, &DataManager::OnUpdateConsumptionEntryRequested, m_Worker, &AppDataBase::OnUpdateConsumptionEntryRequested);
    connect(this, &DataManager::OnDeleteUserRequested, m_Worker, &AppDataBase::OnDeleteUserRequested);
    connect(this, &DataManager::OnUpdateUserRequested, m_Worker, &AppDataBase::OnUpdateUserRequested);
    connect(this, &DataManager::OnCreateUserRequested, m_Worker, &AppDataBase::OnCreateUserRequested);
    connect(this, &DataManager::OnLoginVerifyRequested, m_Worker, &AppDataBase::OnLoginVerifyRequested);
    connect(this, &DataManager::OnProcessCSVRequested, m_CsvWorker, &CSVWorker::OnProcessCSVRequested);

    m_Thread->start();
}

void DataManager::ConnectToDatabase()
{
    emit OnConnectToDataBase();
}

void DataManager::RequestDBState()
{
    emit OnDataBaseRequested();
}

void DataManager::RequestTrySettingsAnonymous()
{
    emit OnTrySettingsAnonymousRequested();
}

void DataManager::RequestTrySettingsWithUser()
{
    emit OnTrySettingsWithUserRequested();
}

void DataManager::RequestUsersList()
{
    emit OnUsersListRequested();
}

void DataManager::RequestAuditLog(QDate DayFilter)
{
    emit OnAuditLogRequested(DayFilter);
}

void DataManager::RequestAppendAuditLog(const QString& Login, const QString& Action)
{
    emit OnAppendAuditLogRequested(Login, Action);
}

void DataManager::RequestConsumerGroupsList()
{
    emit OnConsumerGroupsListRequested();
}

void DataManager::RequestConsumptionDataForGroupAndDate(int GroupId, const QDate& Date)
{
    emit OnConsumptionDataForGroupAndDateRequested(GroupId, Date);
}

void DataManager::RequestForecastDataForDateAndHour(const QDate& Date, int Hour)
{
    emit OnForecastDataForDateAndHourRequested(Date, Hour);
}

void DataManager::RequestGenerateForecastForDateAndHour(const QDate& Date, int Hour)
{
    emit OnGenerateForecastForDateAndHourRequested(Date, Hour);
}

void DataManager::RequestUpdateConsumerGroupAmount(const QString& GroupName, int Amount)
{
    emit OnUpdateConsumerGroupAmountRequested(GroupName, Amount);
}

void DataManager::RequestUpdateConsumptionEntry(int GroupId, const QDate& Date, int Hour, int ConsumerAmount,
                                                double Consumption)
{
    emit OnUpdateConsumptionEntryRequested(GroupId, Date, Hour, ConsumerAmount, Consumption);
}

void DataManager::RequestDeleteUser(const QString& Login)
{
    emit OnDeleteUserRequested(Login);
}

void DataManager::RequestUpdateUser(const QString& Login, const QString& Name, const QString& Password)
{
    emit OnUpdateUserRequested(Login, Name, Password);
}

void DataManager::RequestCreateUser(const QString& Login, const QString& Name, const QString& Role,
                                    const QString& PasswordHash)
{
    emit OnCreateUserRequested(Login, Name, Role, PasswordHash);
}

void DataManager::RequestLoginVerify(const QString& Login, const QString& Password)
{
    emit OnLoginVerifyRequested(Login, Password);
}

void DataManager::RequestProcessCSV(const QString& FilePath, const QString& AnalystLogin)
{
    emit OnProcessCSVRequested(FilePath, AnalystLogin);
}
