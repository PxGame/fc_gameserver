#ifndef DATABASE_H
#define DATABASE_H

# include "pub.h"
# include "settinginfo.h"

using namespace std;
using namespace sql;

struct RankItem
{
    string username;
    int score;
};

class Database
        : public enable_shared_from_this<Database>
{
protected:

    static shared_ptr<Database> m_database;

public:

    static void Create(DbSetting setting);
    static void Destory();
    static inline shared_ptr<Database> GetInstance(){return m_database;}

public:

    Database(DbSetting setting);
    ~Database();

private:
    mutex m_mtDriver;
    mutex m_mtPool;

    DbSetting m_setting;

    Driver* m_driver;

    list<Connection*> m_disableConn;
    list<Connection*> m_enableConn;

private:
    void Init();

    Connection* Get();
    void Release(Connection*& conn);

    Connection* Create();
    void Destory(Connection*& conn);
    void DestoryAll();

public:
    void AddUser(string gameid, string deviceid, string username, string devicetype, string devicemodel, int auth);
    void AddRank(string gameid, string deviceid, int level, int score, int cleartime, string ip);
    list<RankItem> QueryRank(string gameid, int level, int cnt);
};

#endif // DATABASE_H
