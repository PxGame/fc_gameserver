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
        string_t key;
        string_t crt;
    }ssl;
};

struct DbSetting
{
    string_t host;
    string_t username;
    string_t password;
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
