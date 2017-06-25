#ifndef DATABASE_H
#define DATABASE_H

# include "pub.h"
# include "settinginfo.h"

using namespace std;
using namespace sql;

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
};

#endif // DATABASE_H
