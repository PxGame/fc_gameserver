#include "database.h"

shared_ptr<Database> Database::m_database = nullptr;

void Database::Create(DbSetting setting)
{
    try
    {
        m_database = make_shared<Database>(setting);
        m_database->Init();
    }
    catch (const std::exception& e)
    {
        throw e;
    }
}

void Database::Destory()
{
    m_database.reset();
}

Database::Database(DbSetting setting)
{
    //get setting
    m_setting = setting;
}

Database::~Database()
{
    DestoryAll();
}

void Database::Init()
{
    unique_lock<std::mutex> lck(m_mtDriver);

    m_driver = get_driver_instance();
}

Connection *Database::Get()
{
    unique_lock<std::mutex> lck(m_mtPool);
    Connection* conn = nullptr;

    try
    {
        int reconnCnt = 0;
        do
        {
            if (0 < m_disableConn.size())
            {
                conn = m_disableConn.front();
                m_disableConn.pop_front();
            }
            else
            {
                conn = Create();
            }

            if (nullptr != conn && conn->isValid())
            {
                m_enableConn.push_back(conn);
                break;
            }

            Destory(conn);

            reconnCnt++;

        } while (reconnCnt < m_setting.reconnmaxsize);

        if (nullptr == conn)
        {//connection is invaild excption
            throw runtime_error("connection is invaild.");
        }
    }
    catch (const std::exception& e)
    {
        Destory(conn);
        throw e;
    }

    return conn;
}

void Database::Release(Connection *&conn)
{
    unique_lock<std::mutex> lck(m_mtPool);

    if(nullptr == conn)
    {
        return;
    }

    if(m_disableConn.size() >= m_setting.connpoolsize)
    {
        Destory(conn);
        return;
    }

    if(!conn->isValid())
    {
        Destory(conn);
        return;
    }

    m_disableConn.push_back(conn);
}

Connection *Database::Create()
{
    unique_lock<std::mutex> lck(m_mtDriver);

    Connection* conn = nullptr;

    try
    {
        conn = m_driver->connect(m_setting.host, m_setting.username, m_setting.password);
    }
    catch (const std::exception& e)
    {
        throw e;
    }

    return conn;
}

void Database::Destory(Connection *&conn)
{
    if (nullptr == conn)
    {
        return;
    }

    delete(conn);
    conn = nullptr;
}

void Database::DestoryAll()
{
    unique_lock<std::mutex> lck(m_mtPool);

    for(Connection* conn : m_disableConn)
    {
        Destory(conn);
    }
    for(Connection* conn : m_enableConn)
    {
        Destory(conn);
    }
}
