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


        if (1 < res->rowsCount())
        {
            throw new runtime_error("call adduser result is more than 1.");
        }

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

shared_ptr<UserItem> Database::QueryUser(string gameid, string deviceid)
{
    shared_ptr<UserItem> item = nullptr;
    try
    {
        shared_ptr<Connection> conn(Get(), [this](Connection* ptr){Release(ptr);});

        shared_ptr<PreparedStatement> stmt;
        shared_ptr<ResultSet> res;

        stmt.reset(conn->prepareStatement(
                       "call queryuser(?,?)"));

        stmt->setString(1, gameid.c_str());
        stmt->setString(2, deviceid.c_str());

        res.reset(stmt->executeQuery());

        if (1 < res->rowsCount())
        {
            throw new runtime_error("call QueryUser result is more than 1.");
        }

        for (;;)
        {
            while (res->next()) {
                if(nullptr == item)
                {
                    item = make_shared<UserItem>();
                }
                item->username = res->getString("username").asStdString();
                item->auth = res->getInt("auth");
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

    return item;
}

void Database::DeleteUser(string gameid, string deviceid)
{

    try
    {
        shared_ptr<Connection> conn(Get(), [this](Connection* ptr){Release(ptr);});

        shared_ptr<PreparedStatement> stmt;
        shared_ptr<ResultSet> res;

        stmt.reset(conn->prepareStatement(
                       "call deleteuser(?,?)"));

        stmt->setString(1, gameid.c_str());
        stmt->setString(2, deviceid.c_str());

        res.reset(stmt->executeQuery());

        if (1 < res->rowsCount())
        {
            throw new runtime_error("call deleteuser result is more than 1.");
        }

        for (;;)
        {
            while (res->next()) {
                int errcode = res->getInt("errcode");
                if (errcode != 0)
                {
                    std::stringstream ostr;
                    ostr << "call deleteuser error code : " << errcode;
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
        stmt->setString(3, ip.c_str());
        stmt->setInt(4, level);
        stmt->setInt(5, score);
        stmt->setInt(6, cleartime);

        res.reset(stmt->executeQuery());

        if (1 < res->rowsCount())
        {
            throw new runtime_error("call addrank result is more than 1.");
        }

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

shared_ptr<RankNumber> Database::AddRankEx(string gameid, string deviceid, int level, int score, int cleartime, string ip)
{
    shared_ptr<RankNumber> rankNum;
    try
    {
        shared_ptr<Connection> conn(Get(), [this](Connection* ptr){Release(ptr);});

        shared_ptr<PreparedStatement> stmt;
        shared_ptr<ResultSet> res;

        stmt.reset(conn->prepareStatement(
                       "call addrankex(?,?,?,?,?,?)"));

        stmt->setString(1, gameid.c_str());
        stmt->setString(2, deviceid.c_str());
        stmt->setString(3, ip.c_str());
        stmt->setInt(4, level);
        stmt->setInt(5, score);
        stmt->setInt(6, cleartime);

        res.reset(stmt->executeQuery());

        if (1 < res->rowsCount())
        {
            throw new runtime_error("call addrankex result is more than 1.");
        }

        for (;;)
        {
            while (res->next()) {

                if (0 != res->findColumn("errcode"))
                {
                    int errcode = res->getInt("errcode");
                    std::stringstream ostr;
                    ostr << "call addrankex error code : " << errcode;
                    throw runtime_error(ostr.str());
                }

                int current = res->getInt("current");
                int good = res->getInt("best");

                rankNum = make_shared<RankNumber>();
                rankNum->current = current;
                rankNum->best = good;
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

    return rankNum;
}

shared_ptr<list<shared_ptr<RankItem>>> Database::QueryRank(string gameid, int level, int cnt)
{
    shared_ptr<list<shared_ptr<RankItem>>> items = make_shared<list<shared_ptr<RankItem>>>();
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
                shared_ptr<RankItem> item = make_shared<RankItem>();

                item->username = res->getString("username").asStdString();
                item->score = res->getInt("score");

                items->push_back(item);
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
