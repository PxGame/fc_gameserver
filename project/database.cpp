#include "database.h"

shared_ptr<Database> Database::m_database = nullptr;

void Database::Create(DbSetting setting)
{
    try
    {
        m_database = make_shared<Database>(setting);
        m_database->Init();
    }
    catch (const exception& e)
    {
        std::stringstream ostr;
        ostr << "[db]" << e.what();
        throw runtime_error(ostr.str());
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
            throw runtime_error("[db]connection is invaild.");
        }
    }
    catch (const exception& e)
    {
        Destory(conn);
        std::stringstream ostr;
        ostr << "[db]" << e.what();
        throw runtime_error(ostr.str());
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
    catch (const exception& e)
    {
        std::stringstream ostr;
        ostr << "[db]" << e.what();
        throw runtime_error(ostr.str());
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

    for(Connection*& conn : m_disableConn)
    {
        Destory(conn);
    }
    for(Connection*& conn : m_enableConn)
    {
        Destory(conn);
    }
}

//db operations =====================================================================

void Database::AddUser(string gameid, string deviceid, string username, string devicetype, string devicemodel, int auth)
{
    try
    {
        shared_ptr<Connection> conn(Get(), [this](Connection* ptr){Release(ptr);});

        shared_ptr<PreparedStatement> stmt;
        shared_ptr<ResultSet> res;

        stmt.reset(conn->prepareStatement(
                       "call adduser(?,?,?,?,?,?)"));

        stmt->setString(1, gameid.c_str());
        stmt->setString(2, deviceid.c_str());
        stmt->setString(3, username.c_str());
        stmt->setString(4, devicetype.c_str());
        stmt->setString(5, devicemodel.c_str());
        stmt->setInt(6, auth);

        res.reset(stmt->executeQuery());

        for (;;)
        {
            while (res->next()) {
                int errcode = res->getInt("errcode");
                if (errcode != 0)
                {
                    std::stringstream ostr;
                    ostr << "call adduser error code : " << errcode;
                    throw runtime_error(ostr.str());
                }
                return;
            }

            if (stmt->getMoreResults())
            {
                res.reset(stmt->getResultSet());
                continue;
            }
            break;
        }
    }
    catch (const exception& e)
    {
        std::stringstream ostr;
        ostr << "[db]" << e.what();
        throw runtime_error(ostr.str());
    }
}

void Database::AddRank(string gameid, string deviceid, int level, int score, int cleartime, string ip)
{
    try
    {
        shared_ptr<Connection> conn(Get(), [this](Connection* ptr){Release(ptr);});

        shared_ptr<PreparedStatement> stmt;
        shared_ptr<ResultSet> res;

        stmt.reset(conn->prepareStatement(
                       "call addrank(?,?,?,?,?,?)"));

        stmt->setString(1, gameid.c_str());
        stmt->setString(2, deviceid.c_str());
        stmt->setInt(3, level);
        stmt->setInt(4, score);
        stmt->setInt(5, cleartime);
        stmt->setString(6, ip.c_str());

        res.reset(stmt->executeQuery());

        for (;;)
        {
            while (res->next()) {
                int errcode = res->getInt("errcode");
                if (errcode != 0)
                {
                    std::stringstream ostr;
                    ostr << "call addrank error code : " << errcode;
                    throw runtime_error(ostr.str());
                }
                return;
            }

            if (stmt->getMoreResults())
            {
                res.reset(stmt->getResultSet());
                continue;
            }
            break;
        }
    }
    catch (const exception& e)
    {
        std::stringstream ostr;
        ostr << "[db]" << e.what();
        throw runtime_error(ostr.str());
    }
}

list<RankItem> Database::QueryRank(string gameid, int level, int cnt)
{
    list<RankItem> items;
    try
    {
        shared_ptr<Connection> conn(Get(), [this](Connection* ptr){Release(ptr);});

        shared_ptr<PreparedStatement> stmt;
        shared_ptr<ResultSet> res;

        stmt.reset(conn->prepareStatement(
                       "call queryrank(?,?,?)"));

        stmt->setString(1, gameid.c_str());
        stmt->setInt(2, level);
        stmt->setInt(3, cnt);

        res.reset(stmt->executeQuery());

        for (;;)
        {
            while (res->next()) {
                RankItem item;
                item.username = res->getString("username").asStdString();
                item.score = res->getInt("score");

                items.push_back(item);
            }

            if (stmt->getMoreResults())
            {
                res.reset(stmt->getResultSet());
                continue;
            }
            break;
        }
    }
    catch (const exception& e)
    {
        std::stringstream ostr;
        ostr << "[db]" << e.what();
        throw runtime_error(ostr.str());
    }

    return items;
}
