#include "DataBaseManager.h"

#include "AppDataBase.h"

#include <QMetaObject>
#include <QThread>

DataBaseManager& DataBaseManager::instance()
{
    static DataBaseManager inst;
    return inst;
}

DataBaseManager::DataBaseManager(QObject *parent)
    : QObject{parent}
{}

DataBaseManager::~DataBaseManager()
{
    if (m_thread) {
        m_thread->quit();
        m_thread->wait();
    }
    delete m_worker;
    m_worker = nullptr;
    m_thread = nullptr;
}

void DataBaseManager::Init()
{
    if (m_initialized) {
        return;
    }
    m_initialized = true;

    m_thread = new QThread(this);
    m_worker = new AppDataBase();
    m_worker->moveToThread(m_thread);

    // Worker -> Manager
    connect(m_worker, &AppDataBase::OnDataBaseStateSended, this, &DataBaseManager::OnDataBaseStateSended);
    connect(m_worker, &AppDataBase::OnLostConnection, this, &DataBaseManager::OnLostConnection);
    connect(m_worker, &AppDataBase::SettingsAnonymousResult, this, &DataBaseManager::SettingsAnonymousResult);
    connect(m_worker, &AppDataBase::SettingsUserResult, this, &DataBaseManager::SettingsUserResult);
    connect(m_worker, &AppDataBase::UsersListLoaded, this, &DataBaseManager::UsersListLoaded);
    connect(m_worker, &AppDataBase::UserDeleteFinished, this, &DataBaseManager::UserDeleteFinished);
    connect(m_worker, &AppDataBase::UserUpdateFinished, this, &DataBaseManager::UserUpdateFinished);
    connect(m_worker, &AppDataBase::UserCreateFinished, this, &DataBaseManager::UserCreateFinished);
    connect(m_worker, &AppDataBase::LoginVerifyFinished, this, &DataBaseManager::LoginVerifyFinished);

    // Manager -> Worker
    connect(this, &DataBaseManager::OnConnectToDataBase, m_worker, &AppDataBase::OnConnectRequested);
    connect(this, &DataBaseManager::OnDataBaseRequested, m_worker, &AppDataBase::SendDataBaseState);
    connect(this, &DataBaseManager::OnTrySettingsAnonymousRequested, m_worker, &AppDataBase::OnTrySettingsAnonymous);
    connect(this, &DataBaseManager::OnTrySettingsWithUserRequested, m_worker, &AppDataBase::OnTrySettingsWithUser);
    connect(this, &DataBaseManager::OnUsersListRequested, m_worker, &AppDataBase::GetAllUsers);
    connect(this, &DataBaseManager::OnDeleteUserRequested, m_worker, &AppDataBase::OnDeleteUserRequested);
    connect(this, &DataBaseManager::OnUpdateUserRequested, m_worker, &AppDataBase::OnUpdateUserRequested);
    connect(this, &DataBaseManager::OnCreateUserRequested, m_worker, &AppDataBase::OnCreateUserRequested);
    connect(this, &DataBaseManager::OnLoginVerifyRequested, m_worker, &AppDataBase::OnLoginVerifyRequested);

    m_thread->start();
}

void DataBaseManager::ConnectToDatabase()
{
    emit OnConnectToDataBase();
}

void DataBaseManager::RequestDBState()
{
    emit OnDataBaseRequested();
}

void DataBaseManager::RequestTrySettingsAnonymous()
{
    emit OnTrySettingsAnonymousRequested();
}

void DataBaseManager::RequestTrySettingsWithUser()
{
    emit OnTrySettingsWithUserRequested();
}

void DataBaseManager::RequestUsersList()
{
    emit OnUsersListRequested();
}

void DataBaseManager::RequestDeleteUser(const QString& login)
{
    emit OnDeleteUserRequested(login);
}

void DataBaseManager::RequestUpdateUser(const QString& login, const QString& name, const QString& password)
{
    emit OnUpdateUserRequested(login, name, password);
}

void DataBaseManager::RequestCreateUser(const QString& login, const QString& name, const QString& role,
                                        const QString& passwordHash)
{
    emit OnCreateUserRequested(login, name, role, passwordHash);
}

void DataBaseManager::RequestLoginVerify(const QString& login, const QString& password)
{
    emit OnLoginVerifyRequested(login, password);
}
