#ifndef SETTINGINFO_H
#define SETTINGINFO_H

# include "pub.h"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::json;
using namespace concurrency::streams;
using namespace std;

struct WebSetting
{
    web::uri uri;
    int timeout;
    struct _ssl{
        string key;
        string crt;
        bool use;

        boost::asio::const_buffer KeyBuffer()
        {
            return boost::asio::const_buffer(key.c_str(), key.size());
        }
        boost::asio::const_buffer CrtBuffer()
        {
            return boost::asio::const_buffer(crt.c_str(), crt.size());
        }
    }ssl;
};

struct DbSetting
{
    string host;
    string username;
    string password;
    int connpoolsize;
    int reconnmaxsize;
};

class SettingInfo
{
public:
    SettingInfo();
    void Load(string filePath);

public:
    WebSetting web;
    DbSetting db;
};

#endif // SETTINGINFO_H
