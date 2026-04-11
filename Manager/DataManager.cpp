#include "DataManager.h"

#include "Worker/AppDataBase.h"
#include "Worker/CSVWorker.h"

#include <QMetaObject>
#include <QThread>

DataManager& DataManager::instance()
{
    static DataManager inst;
    return inst;
}

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{}

DataManager::~DataManager()
{
    if (m_thread) {
        m_thread->quit();
        m_thread->wait();
    }
    delete m_CSVWorker;
    m_CSVWorker = nullptr;
    delete m_worker;
    m_worker = nullptr;
    m_thread = nullptr;
}

void DataManager::Init()
{
    if (m_initialized) {
        return;
    }
    m_initialized = true;

    m_thread = new QThread(this);
    m_worker = new AppDataBase();
    m_worker->moveToThread(m_thread);

    m_CSVWorker = new CSVWorker();
    m_CSVWorker->moveToThread(m_thread);

    // Worker -> Manager
    connect(m_worker, &AppDataBase::OnDataBaseStateSended, this, &DataManager::OnDataBaseStateSended);
    connect(m_worker, &AppDataBase::OnLostConnection, this, &DataManager::OnLostConnection);
    connect(m_worker, &AppDataBase::SettingsAnonymousResult, this, &DataManager::SettingsAnonymousResult);
    connect(m_worker, &AppDataBase::SettingsUserResult, this, &DataManager::SettingsUserResult);
    connect(m_worker, &AppDataBase::UsersListLoaded, this, &DataManager::UsersListLoaded);
    connect(m_worker, &AppDataBase::UserDeleteFinished, this, &DataManager::UserDeleteFinished);
    connect(m_worker, &AppDataBase::UserUpdateFinished, this, &DataManager::UserUpdateFinished);
    connect(m_worker, &AppDataBase::UserCreateFinished, this, &DataManager::UserCreateFinished);
    connect(m_worker, &AppDataBase::LoginVerifyFinished, this, &DataManager::LoginVerifyFinished);
    connect(m_CSVWorker, &CSVWorker::CSVProcessingFinished, this, &DataManager::CSVProcessingFinished);

    // Manager -> Worker
    connect(this, &DataManager::OnConnectToDataBase, m_worker, &AppDataBase::OnConnectRequested);
    connect(this, &DataManager::OnDataBaseRequested, m_worker, &AppDataBase::SendDataBaseState);
    connect(this, &DataManager::OnTrySettingsAnonymousRequested, m_worker, &AppDataBase::OnTrySettingsAnonymous);
    connect(this, &DataManager::OnTrySettingsWithUserRequested, m_worker, &AppDataBase::OnTrySettingsWithUser);
    connect(this, &DataManager::OnUsersListRequested, m_worker, &AppDataBase::GetAllUsers);
    connect(this, &DataManager::OnDeleteUserRequested, m_worker, &AppDataBase::OnDeleteUserRequested);
    connect(this, &DataManager::OnUpdateUserRequested, m_worker, &AppDataBase::OnUpdateUserRequested);
    connect(this, &DataManager::OnCreateUserRequested, m_worker, &AppDataBase::OnCreateUserRequested);
    connect(this, &DataManager::OnLoginVerifyRequested, m_worker, &AppDataBase::OnLoginVerifyRequested);
    connect(this, &DataManager::OnProcessCSVRequested, m_CSVWorker, &CSVWorker::OnProcessCSVRequested);

    m_thread->start();
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

void DataManager::RequestDeleteUser(const QString& login)
{
    emit OnDeleteUserRequested(login);
}

void DataManager::RequestUpdateUser(const QString& login, const QString& name, const QString& password)
{
    emit OnUpdateUserRequested(login, name, password);
}

void DataManager::RequestCreateUser(const QString& login, const QString& name, const QString& role,
                                    const QString& passwordHash)
{
    emit OnCreateUserRequested(login, name, role, passwordHash);
}

void DataManager::RequestLoginVerify(const QString& login, const QString& password)
{
    emit OnLoginVerifyRequested(login, password);
}

void DataManager::RequestProcessCSV(const QString& filePath)
{
    emit OnProcessCSVRequested(filePath);
}
